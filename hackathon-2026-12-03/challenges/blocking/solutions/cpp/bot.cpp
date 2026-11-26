// Blocking — C++ (submitted), v1 biggest piece first. Port of solutions/python/bot.py.
// The referee lists every valid move; score = SIZE_WEIGHT * squares of the
// shape - distance of the anchor cell to the centre (big pieces score now and
// cannot be placed later; the centre keeps corners open). Ties: first move.
// CodinGame compiles without -O2: the pragma is mandatory (ALGO.md §0.4).
#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
using namespace std;

constexpr long long SIZE_WEIGHT = 100;

int main() {
    ios::sync_with_stdio(false);
    int shapeCount;
    if (!(cin >> shapeCount)) return 0;
    map<char, int> sizes;
    for (int i = 0; i < shapeCount; ++i) {
        string letter, definition;
        int cols, rows;
        cin >> letter >> cols >> rows >> definition;
        sizes[letter[0]] = (int)count(definition.begin(), definition.end(), '#');
    }
    int players, me, boardSize;
    string letters, firstRow;
    cin >> players >> me >> boardSize >> letters;
    int size = 0;
    while (cin >> firstRow) {
        if (!size) size = (int)firstRow.size();
        string skip;
        for (int i = 0; i < size - 1; ++i) cin >> skip;
        int played;
        cin >> played;
        for (int i = 0; i < 4 * played; ++i) cin >> skip;
        int count;
        cin >> count;
        bool found = false;
        long long bestScore = 0;
        string best = "0 0 A001";
        for (int i = 0; i < count; ++i) {
            int col, row;
            string shape;
            cin >> col >> row >> shape;
            // doubled distance to the centre keeps everything integer
            long long distance = abs(2 * col - (size - 1)) + abs(2 * row - (size - 1));
            auto it = sizes.find(shape[0]);
            long long score = 2 * SIZE_WEIGHT * (it == sizes.end() ? 0 : it->second) - distance;
            if (!found || score > bestScore) {
                found = true;
                bestScore = score;
                best = to_string(col) + " " + to_string(row) + " " + shape;
            }
        }
        if (!cin) return 0;
        cout << best << endl;
    }
}
