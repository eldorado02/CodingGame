#!/usr/bin/env python3
"""Platinum Rift 2 — Python reference of the differential fuzz, v1 spread and march.

The HQs are the zones owned on the first turn. Each turn, zone by zone
(increasing id), our pods:
- stay when they are fighting (fleeing may only go to non-enemy zones);
- at our HQ, GUARD_MARGIN + the visible enemy pods next to it stay home;
- one pod (or enough to outnumber the visible enemies) goes to each
  neighbour we do not own, best first: more platinum (remembered through
  the fog), closer to the enemy HQ, lower id;
- the rest marches one step towards the enemy HQ (BFS distance).
"""
import sys
from collections import deque

GUARD_MARGIN = 1      # spare defenders when enemies stand next to our HQ
UNREACHABLE = 1 << 20


def bfs(adj, start):
    dist = [UNREACHABLE] * len(adj)
    if start < 0:
        return dist
    dist[start] = 0
    queue = deque([start])
    while queue:
        z = queue.popleft()
        for n in adj[z]:
            if dist[n] == UNREACHABLE:
                dist[n] = dist[z] + 1
                queue.append(n)
    return dist


class Game:
    def __init__(self, me, adj):
        self.me = me
        self.adj = adj
        self.platinum = [0] * len(adj)    # last seen platinum per zone
        self.my_hq = -1
        self.enemy_hq = -1
        self.dist = [UNREACHABLE] * len(adj)

    def observe(self, zones):
        for z, owner, pods, visible, plat in zones:
            if visible:
                self.platinum[z] = plat
        if self.my_hq < 0:
            for z, owner, *_ in zones:
                if owner == self.me and self.my_hq < 0:
                    self.my_hq = z
                elif owner == 1 - self.me and self.enemy_hq < 0:
                    self.enemy_hq = z
            self.dist = bfs(self.adj, self.enemy_hq)

    def orders(self, zones):
        owner = [o for _, o, _, _, _ in zones]
        mine = [p[self.me] for _, _, p, _, _ in zones]
        theirs = [p[1 - self.me] for _, _, p, _, _ in zones]
        moves = []
        for z in range(len(zones)):
            if mine[z] == 0 or theirs[z] > 0:
                continue
            available = mine[z] - self.guards(z, theirs)
            available = self.spread(z, available, owner, theirs, moves)
            self.march(z, available, moves)
        return moves

    def guards(self, z, theirs):
        """Pods kept home: the enemies next to our HQ plus a margin."""
        if z != self.my_hq:
            return 0
        threat = sum(theirs[n] for n in self.adj[z])
        return threat + GUARD_MARGIN if threat else 0

    def spread(self, z, available, owner, theirs, moves):
        targets = sorted((n for n in self.adj[z] if owner[n] != self.me),
                         key=lambda n: (-self.platinum[n], self.dist[n], n))
        for n in targets:
            if available <= 0:
                break
            send = min(available, theirs[n] + 1)
            moves.append((send, z, n))
            available -= send
        return available

    def march(self, z, available, moves):
        if available <= 0:
            return
        best = -1
        for n in self.adj[z]:
            if self.dist[n] < self.dist[z] and (best < 0 or self.dist[n] < self.dist[best]):
                best = n
        if best >= 0:
            moves.append((available, z, best))


def main():
    tokens = (int(t) for line in iter(sys.stdin.readline, "") for t in line.split())
    try:
        _, me, zone_count, link_count = (next(tokens) for _ in range(4))
        for _ in range(2 * zone_count):
            next(tokens)
        adj = [[] for _ in range(zone_count)]
        for _ in range(link_count):
            a, b = next(tokens), next(tokens)
            adj[a].append(b)
            adj[b].append(a)
        adj = [sorted(set(n)) for n in adj]
        game = Game(me, adj)
        while True:
            next(tokens)    # our platinum stock: pods are bought automatically
            zones = []
            for _ in range(zone_count):
                z, owner, p0, p1, visible, plat = (next(tokens) for _ in range(6))
                zones.append((z, owner, (p0, p1), visible, plat))
            zones.sort()
            game.observe(zones)
            moves = game.orders(zones)
            print(" ".join("%d %d %d" % m for m in moves) if moves else "WAIT")
            print("WAIT", flush=True)
    except StopIteration:
        return


if __name__ == "__main__":
    main()
