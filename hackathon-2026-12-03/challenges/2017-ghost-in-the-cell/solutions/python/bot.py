#!/usr/bin/env python3
"""Ghost in the Cell (Wood 4) — time-aware heuristics + multi-order turns.

Inspired by Agade's contest-winning postmortem:
  value/(turns^2 * required_force), spare units after 5-turn defense,
  state updated after each order, early bombs on high-production targets.
"""
import sys
from math import sqrt

INF = 10 ** 6
HORIZON = 6


def floyd(n, links):
    d = [[INF] * n for _ in range(n)]
    for i in range(n):
        d[i][i] = 0
    for a, b, w in links:
        if w < d[a][b]:
            d[a][b] = d[b][a] = w
    for k in range(n):
        dk = d[k]
        for i in range(n):
            di = d[i]
            dik = di[k]
            for j in range(n):
                nd = dik + dk[j]
                if nd < di[j]:
                    di[j] = nd
    # centrality: number of shortest paths passing through each factory
    through = [0] * n
    for i in range(n):
        for k in range(n):
            if i == k:
                continue
            for j in range(n):
                if j == i or j == k:
                    continue
                if d[i][j] == d[i][k] + d[k][j]:
                    through[k] += 1
    return d, through


class State:
    """Projected state; `mine`/`hostile` are signed from our point of view."""

    def __init__(self, owner, units, prod, block):
        self.owner = owner[:]      # 1 ours, -1 enemy, 0 neutral
        self.units = units[:]
        self.prod = prod[:]
        self.block = block[:]      # turns before production resumes

    def project(self, i, t, arrivals):
        """Return (owner, mine, hostile) at factory i after t turns."""
        own = self.units[i] if self.owner[i] == 1 else 0
        hostile = self.units[i] if self.owner[i] != 1 else 0
        ownr = self.owner[i]
        block = self.block[i]
        for turn in range(1, t + 1):
            for a in arrivals:
                if a[2] == i and a[4] == turn:
                    if a[0] == 1:
                        own += a[3]
                    else:
                        hostile += a[3]
            suppressed = block > 0
            for dst, eta in getattr(self, "bomb_events", ()):
                if dst == i and eta <= turn <= eta + 4:
                    suppressed = True
            if not suppressed and ownr != 0:
                if ownr == 1:
                    own += self.prod[i]
                else:
                    hostile += self.prod[i]
            for dst, eta in getattr(self, "bomb_events", ()):
                if dst == i and eta == turn:
                    victim = own if ownr == 1 else hostile
                    loss = max(10, victim // 2)
                    if ownr == 1:
                        own = max(0, own - loss)
                    else:
                        hostile = max(0, hostile - loss)
            if block > 0:
                block -= 1
            if own > hostile:
                ownr, own, hostile = 1, own - hostile, 0
            elif hostile > own:
                ownr, own, hostile = -1, 0, hostile - own
            else:
                own = hostile = 0
                if ownr == 0:
                    ownr = 0
        return ownr, own, hostile

    def defense_need(self, i, arrivals, horizon=HORIZON):
        """Extra units needed at i to hold it for `horizon` turns."""
        if self.owner[i] != 1:
            return 0
        need = 0
        for t in range(1, horizon + 1):
            _o, mine, hostile = self.project(i, t, arrivals)
            if hostile > mine:
                need = max(need, hostile + 1 - mine)
        return need

    def capture_need(self, i, eta, arrivals):
        """Hostile garrison we must beat on the turn our troop arrives."""
        _o, mine, hostile = self.project(i, eta, arrivals)
        if mine > hostile:
            return 0  # already captured by troops currently in flight
        return hostile + 1


def decide(state, dist, value, n, turn, bombs_left, orders, arrivals):
    """Build as many good orders as possible, updating `arrivals` after each."""

    # ---- 1. Defensive reinforcements, most valuable threatened factory first ----
    threatened = [i for i in range(n) if state.owner[i] == 1]
    threatened.sort(key=lambda i: -value[i])
    for dst in threatened:
        for _ in range(2):
            need = state.defense_need(dst, arrivals)
            if need <= 0:
                break
            helpers = [s for s in range(n)
                       if s != dst and state.owner[s] == 1
                       and dist[s][dst] < INF and dist[s][dst] <= need + 2]
            helpers.sort(key=lambda s: (dist[s][dst], -state.units[s]))
            sent = False
            for src in helpers:
                used = sum(a[3] for a in arrivals if a[1] == src)
                spare = state.units[src] - used - state.defense_need(src, arrivals)
                if spare <= 0:
                    continue
                cnt = min(spare, need)
                if cnt <= 0:
                    continue
                eta = dist[src][dst]
                arrivals.append((1, src, dst, cnt, eta))
                orders.append("MOVE %d %d %d" % (src, dst, cnt))
                need -= cnt
                sent = True
                if need <= 0:
                    break
            if not sent:
                break

    # ---- 2. Attacks and scoring ----
    for _ in range(40):
        best = None
        for src in range(n):
            if state.owner[src] != 1:
                continue
            used = sum(a[3] for a in arrivals if a[1] == src)
            spare = state.units[src] - used - state.defense_need(src, arrivals)
            if spare <= 0:
                continue
            for dst in range(n):
                if dst == src or state.owner[dst] == 1 or dist[src][dst] >= INF:
                    continue
                eta = dist[src][dst]
                need = state.capture_need(dst, eta, arrivals)
                if state.owner[dst] == 0:
                    # neutral: only send what is useful; partial sends combine
                    cnt = min(spare, need)
                    if cnt < 1:
                        continue
                    score = value[dst] / ((eta + 0.35) ** 2 * max(2.0, need ** 0.7))
                else:
                    # enemy: overkill is less costly than failing to capture
                    if spare < need * 0.7:
                        continue
                    cnt = spare
                    score = value[dst] / ((eta + 0.35) ** 2 * 8.0)
                score += 0.0001 * cnt
                if best is None or score > best[0]:
                    best = (score, src, dst, cnt)
        if best is None:
            break
        _score, src, dst, cnt = best
        arrivals.append((1, src, dst, cnt, dist[src][dst]))
        orders.append("MOVE %d %d %d" % (src, dst, cnt))

    # ---- 3. Bombs ----
    if bombs_left > 0:
        bomb = None
        already_bombed = set(dst for dst, _eta in getattr(state, "bomb_events", ()))
        if turn <= 2:
            # Hit high-production factories the enemy is likely to own soon.
            enemy_srcs = [i for i in range(n) if state.owner[i] == -1]
            if not enemy_srcs:
                enemy_srcs = [i for i in range(n) if state.owner[i] == 0]
            for dst in range(n):
                if state.owner[dst] == 1 or dst in already_bombed:
                    continue
                et = min((dist[e][dst] for e in enemy_srcs if dist[e][dst] < INF),
                         default=INF)
                if et >= INF or et > 10:
                    continue
                if state.prod[dst] <= 0:
                    continue
                score = float(state.prod[dst] ** 3) / max(1, et)
                if bomb is None or score > bomb[0]:
                    bomb = (score, dst)
        else:
            # Later: blunt a large enemy garrison that can threaten us.
            for dst in range(n):
                if state.owner[dst] != -1 or state.units[dst] < 18 or dst in already_bombed:
                    continue
                score = state.units[dst] + 10 * state.prod[dst]
                if bomb is None or score > bomb[0]:
                    bomb = (score, dst)
        if bomb is not None:
            dst = bomb[1]
            srcs = [s for s in range(n) if state.owner[s] == 1 and dist[s][dst] < INF]
            # Never cancel an existing troop order from the same source to dst.
            srcs = [s for s in srcs
                    if not any(o.startswith("MOVE %d %d " % (s, dst)) for o in orders)]
            if srcs:
                src = min(srcs, key=lambda s: dist[s][dst])
                orders.append("BOMB %d %d" % (src, dst))
                bombs_left -= 1
    return bombs_left


def main():
    rd = sys.stdin.readline
    line = rd()
    if not line:
        return
    n = int(line)
    m = int(rd())
    links = [tuple(map(int, rd().split())) for _ in range(m)]
    dist, through = floyd(n, links)
    value = [0.0] * n
    turn = 0
    bombs_left = 2
    while True:
        line = rd()
        if not line:
            return
        cnt = int(line)
        owner = [0] * n
        units = [0] * n
        prod = [0] * n
        block = [0] * n
        arrivals = []
        own_bombs = 0
        bomb_events = []
        for _ in range(cnt):
            p = rd().split()
            if len(p) < 7:
                return
            eid = int(p[0])
            a, b, c, d, _e = map(int, p[2:7])
            if p[1] == "FACTORY":
                owner[eid], units[eid], prod[eid], block[eid] = a, b, c, d
            elif p[1] == "TROOP":
                arrivals.append((a, -1, c, d, _e))
            elif p[1] == "BOMB":
                if a == 1:
                    own_bombs += 1
                    if c >= 0 and d > 0:
                        bomb_events.append((c, d))
        bombs_left = min(bombs_left, 2 - own_bombs)
        for i in range(n):
            value[i] = prod[i] + 0.1 + 0.01 * sqrt(through[i])
        state = State(owner, units, prod, block)
        state.bomb_events = bomb_events
        orders = []
        bombs_left = decide(state, dist, value, n, turn, bombs_left, orders, arrivals)
        print(";".join(orders) if orders else "WAIT", flush=True)
        turn += 1


if __name__ == "__main__":
    main()
