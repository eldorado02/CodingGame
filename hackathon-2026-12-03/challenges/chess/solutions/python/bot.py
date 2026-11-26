#!/usr/bin/env python3
"""Chess (Chess960) — Python reference of the differential fuzz, v1 one-ply with hanging pieces.

First turn: we ask for `fen moves` (board and legal moves). Each legal UCI
move is played on the FEN board (castling = king onto its own rook,
en passant, promotion) and scored for us:
  material + PST_WEIGHT * centrality of minor pieces / pawn advance
  - the worst loss the opponent can inflict next: our attacked piece, minus
    the cheapest attacker when the square is defended
  + CHECK_BONUS when the enemy king is attacked.
Ties: first legal move.
"""
import sys

VALUES = {"p": 100, "n": 320, "b": 330, "r": 500, "q": 900, "k": 0}
KNIGHT = ((1, 2), (2, 1), (2, -1), (1, -2), (-1, -2), (-2, -1), (-2, 1), (-1, 2))
KING = ((1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1), (0, -1), (1, -1))
DIAGONALS = ((1, 1), (1, -1), (-1, 1), (-1, -1))
LINES = ((1, 0), (-1, 0), (0, 1), (0, -1))
CHECK_BONUS = 30
KING_ATTACKER = 10000   # a king only takes an undefended piece
CENTER_WEIGHT = 4       # per step closer to the centre for knights and bishops
PAWN_ADVANCE = 3        # per rank advanced
INPUTS = "fen moves"


def parse_board(field):
    """{(file, rank): piece} from the FEN board field (rank 7 first)."""
    board = {}
    rank = 7
    for row in field.split("/"):
        file = 0
        for ch in row:
            if ch.isdigit():
                file += int(ch)
            else:
                board[(file, rank)] = ch
                file += 1
        rank -= 1
    return board


def is_white(piece):
    return piece.isupper()


def square(text):
    """'e4' -> (4, 3); off-board for anything else."""
    if len(text) < 2 or not text[1].isdigit():
        return (-1, -1)
    return ord(text[0]) - ord("a"), int(text[1]) - 1


def play(board, move, white):
    """Board after the UCI move of the side to move."""
    after = dict(board)
    src, dst = square(move[0:2]), square(move[2:4])
    if not all(0 <= v < 8 for v in src + dst) or src not in after:
        return after
    piece = after.pop(src)
    target = after.get(dst)
    if piece.lower() == "k" and target and is_white(target) == white and target.lower() == "r":
        del after[dst]                       # castling: king onto its own rook
        rank = dst[1]
        king_file, rook_file = (6, 5) if dst[0] > src[0] else (2, 3)
        after[(king_file, rank)] = piece
        after[(rook_file, rank)] = target
        return after
    if piece.lower() == "p" and src[0] != dst[0] and target is None:
        after.pop((dst[0], src[1]), None)    # en passant
    if len(move) > 4:
        piece = move[4].upper() if white else move[4].lower()
    after[dst] = piece
    return after


def attackers(board, sq, white):
    """Values of the pieces of `white` attacking square sq."""
    out = []
    f, r = sq
    pawn_rank = r - 1 if white else r + 1
    for df in (-1, 1):
        p = board.get((f + df, pawn_rank))
        if p and is_white(p) == white and p.lower() == "p":
            out.append(VALUES["p"])
    for steps, kinds, slide in ((KNIGHT, "n", False), (KING, "k", False), (DIAGONALS, "bq", True), (LINES, "rq", True)):
        for df, dr in steps:
            nf, nr = f + df, r + dr
            while 0 <= nf < 8 and 0 <= nr < 8:
                p = board.get((nf, nr))
                if p:
                    if is_white(p) == white and p.lower() in kinds:
                        out.append(VALUES[p.lower()] if p.lower() != "k" else KING_ATTACKER)
                    break
                if not slide:
                    break
                nf, nr = nf + df, nr + dr
    return out


def evaluate(board, white):
    score = 0
    worst_loss = 0
    for sq, p in board.items():
        mine = is_white(p) == white
        value = VALUES[p.lower()]
        f, r = sq
        if p.lower() in "nb":
            value += CENTER_WEIGHT * (6 - (abs(2 * f - 7) + abs(2 * r - 7)) // 2)
        elif p.lower() == "p":
            value += PAWN_ADVANCE * (r if is_white(p) else 7 - r)
        score += value if mine else -value
        if mine and p.lower() != "k":
            hits = attackers(board, sq, not white)
            if hits:
                defended = attackers(board, sq, white)
                loss = VALUES[p.lower()] - min(hits) if defended else VALUES[p.lower()]
                worst_loss = max(worst_loss, loss)
        if not mine and p.lower() == "k" and attackers(board, sq, white):
            score += CHECK_BONUS
    return score - worst_loss


def choose(fen, moves):
    fields = fen.split()
    board = parse_board(fields[0])
    white = len(fields) < 2 or fields[1] == "w"
    best, best_score = None, None
    for move in moves:
        if len(move) < 4:
            continue
        score = evaluate(play(board, move, white), white)
        if best_score is None or score > best_score:
            best, best_score = move, score
    return best or (moves[0] if moves else "random")


def main():
    read = sys.stdin.readline
    count = read()
    if not count.strip():
        return
    for _ in range(int(count)):
        read()
    print(INPUTS, flush=True)
    while True:
        fen = read().strip()
        if not fen:
            return
        moves = [read().strip() for _ in range(int(read()))]
        print(choose(fen, moves), flush=True)


if __name__ == "__main__":
    main()
