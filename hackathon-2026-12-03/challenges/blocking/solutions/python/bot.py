#!/usr/bin/env python3
"""Blocking — Python reference of the differential fuzz, v1 biggest piece first.

The referee lists every valid move (`col row` + oriented shape `Q013`: letter,
flip, rotation, square). Score = SIZE_WEIGHT * squares of the shape - Manhattan
distance of the anchor cell to the board centre (big pieces score now and
cannot be placed later; the centre keeps the most corners open).
Ties: first move of the list.
"""
import sys

SIZE_WEIGHT = 100


def main():
    tokens = (t for line in iter(sys.stdin.readline, "") for t in line.split())
    try:
        sizes = {}
        for _ in range(int(next(tokens))):
            letter, _, _, definition = next(tokens), next(tokens), next(tokens), next(tokens)
            sizes[letter] = definition.count("#")
        for _ in range(3):      # players, our id, board size
            next(tokens)
        next(tokens)            # letters of the shapes of this game
        size = None
        while True:
            first_row = next(tokens)
            size = size or len(first_row)
            for _ in range(size - 1):
                next(tokens)
            for _ in range(int(next(tokens))):
                for _ in range(4):
                    next(tokens)
            moves = []
            for _ in range(int(next(tokens))):
                col, row, shape = int(next(tokens)), int(next(tokens)), next(tokens)
                moves.append((col, row, shape))
            best, best_score = None, None
            for col, row, shape in moves:
                # doubled distance to the centre keeps everything integer
                distance = abs(2 * col - (size - 1)) + abs(2 * row - (size - 1))
                score = 2 * SIZE_WEIGHT * sizes.get(shape[0], 0) - distance
                if best_score is None or score > best_score:
                    best, best_score = (col, row, shape), score
            print("%d %d %s" % best if best else "0 0 A001", flush=True)
    except StopIteration:
        return


if __name__ == "__main__":
    main()
