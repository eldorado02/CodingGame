#!/usr/bin/env python3
"""Génère des cas déterministes Shobu (positions + actions légales)."""
import os
import random
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'solutions', 'python'))
import bot

OUT = os.path.join(os.path.dirname(__file__), 'fuzz')
SEED = 20261122


def random_position(rng):
    """Position issue de coups légaux aléatoires depuis le départ."""
    rows = ['wwwwwwww', '........', '........', 'bbbbbbbb',
            'wwwwwwww', '........', '........', 'bbbbbbbb']
    cells = [0 if ch == 'b' else 1 if ch == 'w' else -1 for row in rows for ch in row]
    me = 0
    for _ in range(rng.randint(0, 25)):
        moves = bot.gen_moves(cells, me)
        if not moves:
            me = 1 - me
            moves = bot.gen_moves(cells, me)
        if not moves:
            break
        m = rng.choice(moves)
        cells = bot.apply_move(cells, me, m)
        if min(bot.stones(cells, b, 1 - me) for b in range(4)) == 0:
            break
        me = 1 - me
    return cells


def render(cells, me, rng):
    rows = []
    for y in range(8):
        rows.append(''.join('b' if cells[y * 8 + x] == 0 else
                            'w' if cells[y * 8 + x] == 1 else '.'
                            for x in range(8)))
    moves = bot.gen_moves(cells, me)
    last = '-' if rng.random() < 0.5 else ' '.join(map(str, rng.choice(moves)))
    lines = ['1 %d' % me] + rows + [last, str(len(moves) + (1 if me == 1 else 0))]
    lines += [' '.join(map(str, m)) for m in moves]
    if me == 1:
        lines.append('SWAP')
    return '\n'.join(lines) + '\n'


def main():
    os.makedirs(OUT, exist_ok=True)
    for f in os.listdir(OUT):
        os.remove(os.path.join(OUT, f))
    rng = random.Random(SEED)
    for i in range(200):
        cells = random_position(rng)
        me = rng.randint(0, 1)
        with open(os.path.join(OUT, 'case_%03d.in' % i), 'w') as fh:
            fh.write(render(cells, me, rng))
    print('generated 200 cases in', OUT)


if __name__ == '__main__':
    main()
