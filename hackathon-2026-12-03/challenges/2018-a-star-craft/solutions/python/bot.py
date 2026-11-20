#!/usr/bin/env python3
"""A*Craft — flèches parcimonieuses : poser seulement quand le droit mène à un état revisité."""
import sys

W, H = 19, 10
DX = [0, 1, 0, -1]
DY = [-1, 0, 1, 0]
CH = "URDL"
LIMIT = 4 * W * H


def main() -> None:
    grid_lines = [sys.stdin.readline().strip() for _ in range(H)]
    if not grid_lines[0]:
        return
    data = grid_lines + [sys.stdin.readline().strip()]
    rc = int(data[H])
    data += [sys.stdin.readline().strip() for _ in range(rc)]
    arrow = [[-1] * W for _ in range(H)]
    void = [[False] * W for _ in range(H)]
    for y in range(H):
        row = data[y].upper()
        for x in range(W):
            c = row[x]
            if c == "#":
                void[y][x] = True
            elif c in CH:
                arrow[y][x] = CH.index(c)

    robots: list[tuple[int, int, int]] = []
    for i in range(rc):
        parts = data[11 + i].split()
        robots.append((int(parts[0]), int(parts[1]), CH.index(parts[2].upper())))
    start = {(x, y) for x, y, _ in robots}

    # Tour 0 : une flèche sous un robot change sa direction initiale.
    # On ne la pose que si le robot foncerait dans le vide.
    initial: list[tuple[int, int, int]] = []
    for i, (x, y, d) in enumerate(robots):
        nx, ny = (x + DX[d]) % W, (y + DY[d]) % H
        if void[ny][nx]:
            for d2 in range(4):
                tx, ty = (x + DX[d2]) % W, (y + DY[d2]) % H
                if not void[ty][tx]:
                    arrow[y][x] = d2
                    robots[i] = (x, y, d2)
                    initial.append((x, y, d2))
                    break

    def build(order: list[int]) -> tuple[int, list[tuple[int, int, int]]]:
        board = [row[:] for row in arrow]
        visits = [[0] * 4 for _ in range(W * H)]
        placed: list[tuple[int, int, int]] = []
        total = 0
        for ri in order:
            x, y, d = robots[ri]
            visits[y * W + x][d] += 1
            for _ in range(LIMIT):
                nx = (x + DX[d]) % W
                ny = (y + DY[d]) % H
                if void[ny][nx]:
                    break
                if board[ny][nx] != -1:
                    nd = board[ny][nx]
                elif (nx, ny) in start:
                    nd = d  # ne pas changer la direction initiale d'un robot
                else:
                    best_key = None
                    nd = d
                    for d2 in range(4):
                        if visits[ny * W + nx][d2] > 0:
                            continue
                        tx = (nx + DX[d2]) % W
                        ty = (ny + DY[d2]) % H
                        look = 99 if void[ty][tx] else sum(visits[ty * W + tx])
                        key = (look, d2)
                        if best_key is None or key < best_key:
                            best_key, nd = key, d2
                    if best_key is None:
                        break
                    if nd != d:
                        board[ny][nx] = nd
                        placed.append((nx, ny, nd))
                if visits[ny * W + nx][nd] > 0:
                    break
                visits[ny * W + nx][nd] += 1
                total += 1
                x, y, d = nx, ny, nd
        return total, placed

    order = list(range(rc))
    s1, p1 = build(order)
    s2, p2 = build(order[::-1])
    extra = p2 if s2 > s1 else p1
    print(" ".join(f"{x} {y} {CH[d]}" for x, y, d in initial + extra), flush=True)


if __name__ == "__main__":
    main()
