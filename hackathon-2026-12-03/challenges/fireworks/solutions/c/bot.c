/* Fireworks (Hanabi) — C port of cpp/bot.cpp (same conventions, same tie-breaks).
 * Compile: gcc -O2 -std=c11 -Wall -Wextra bot.c -o bot
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { COLOR_COUNT = 5, TEAM = 3, MAX_INFO = 12, DECK = 50, HAND = 5, NONE = -1 };
enum { MAX_EVENTS = 256, LINE_SIZE = 128, SLOT_SIZE = 16, MAX_PARTS = 8, MAX_KNOWN = 256, MAX_HAND = 16, MAX_PLAYERS = 8 };
static const char *COLORS[COLOR_COUNT] = {"WHITE", "RED", "BLUE", "GREEN", "YELLOW"};
static const char *SLOTS = "ABCDE";
#define UNKNOWN "?"

typedef struct { int player; char slot[SLOT_SIZE]; int colour, level; } Knowledge;
typedef struct { char slot[SLOT_SIZE]; int colour, level; } Card;
typedef struct { int player, count; Card cards[MAX_HAND]; } Hand;

static int me, deck, fireworks[COLOR_COUNT];
static char my_slots[HAND][SLOT_SIZE];
static int my_slot_count;
static Knowledge known[MAX_KNOWN];
static int known_count;
static Hand hands[MAX_PLAYERS];
static int hand_count;

static bool is_digits(const char *s) {
    if (!*s) return false;
    for (; *s; ++s)
        if (*s < '0' || *s > '9') return false;
    return true;
}

static int colour_index(const char *name) {
    for (int c = 0; c < COLOR_COUNT; ++c)
        if (!strcmp(COLORS[c], name)) return c;
    return NONE;
}

/* Splits at ':' (empty parts kept); returns the part count. */
static int split(char *text, char **parts) {
    int n = 0;
    parts[n++] = text;
    for (char *p = text; *p && n < MAX_PARTS; ++p)
        if (*p == ':') { *p = '\0'; parts[n++] = p + 1; }
    return n;
}

/* "COLOR-N" -> colour and level text, split at the first '-'. */
static void partition(const char *value, char *colour, char *level) {
    const char *dash = strchr(value, '-');
    size_t n = dash ? (size_t)(dash - value) : strlen(value);
    if (n >= LINE_SIZE) n = LINE_SIZE - 1;
    memcpy(colour, value, n);
    colour[n] = '\0';
    snprintf(level, LINE_SIZE, "%s", dash ? dash + 1 : "");
}

static void reset(int player) {
    me = player;
    memset(fireworks, 0, sizeof fireworks);
    deck = DECK - TEAM * HAND;
    my_slot_count = HAND;
    for (int i = 0; i < HAND; ++i) snprintf(my_slots[i], SLOT_SIZE, "%c", SLOTS[i]);
    known_count = 0;
    hand_count = 0;
}

static int find_known(int player, const char *slot) {
    for (int i = 0; i < known_count; ++i)
        if (known[i].player == player && !strcmp(known[i].slot, slot)) return i;
    return NONE;
}

static Knowledge knowledge_of(int player, const char *slot) {
    int i = find_known(player, slot);
    if (i == NONE) return (Knowledge){player, "", NONE, NONE};
    return known[i];
}

static void forget(int player, const char *slot) {
    int i = find_known(player, slot);
    if (i != NONE) known[i] = known[--known_count];
    if (deck > 0) {
        --deck;
    } else if (player == me) {
        for (int k = 0; k < my_slot_count; ++k)
            if (!strcmp(my_slots[k], slot)) {
                for (int j = k; j + 1 < my_slot_count; ++j) strcpy(my_slots[j], my_slots[j + 1]);
                --my_slot_count;
                break;
            }
    }
}

static void learn(int player, const char *slot, const char *value) {
    char colour[LINE_SIZE], level[LINE_SIZE];
    partition(value, colour, level);
    int i = find_known(player, slot);
    if (i == NONE) {
        if (known_count == MAX_KNOWN) return;
        i = known_count++;
        known[i].player = player;
        snprintf(known[i].slot, SLOT_SIZE, "%s", slot);
        known[i].colour = known[i].level = NONE;
    }
    if (strcmp(colour, UNKNOWN) && colour_index(colour) != NONE) known[i].colour = colour_index(colour);
    if (strcmp(level, UNKNOWN) && is_digits(level)) known[i].level = atoi(level);
}

/* Full card line of a teammate: fills player and card. */
static bool visible_card(const char *line, int *player, Card *card) {
    char copy[LINE_SIZE], colour[LINE_SIZE], level[LINE_SIZE], *parts[MAX_PARTS];
    snprintf(copy, sizeof copy, "%s", line);
    int n = split(copy, parts);
    if (n != 4 || !is_digits(parts[0]) || strcmp(parts[1], "CARD") || atoi(parts[0]) == me) return false;
    partition(parts[3], colour, level);
    if (colour_index(colour) == NONE || !is_digits(level)) return false;
    *player = atoi(parts[0]);
    snprintf(card->slot, SLOT_SIZE, "%s", parts[2]);
    card->colour = colour_index(colour);
    card->level = atoi(level);
    return true;
}

static Hand *hand_of(int player, bool create) {
    for (int i = 0; i < hand_count; ++i)
        if (hands[i].player == player) return &hands[i];
    if (!create || hand_count == MAX_PLAYERS) return NULL;
    hands[hand_count] = (Hand){player, 0, {{{0}, 0, 0}}};
    return &hands[hand_count++];
}

/* Stores a card (a later line overrides the same slot), slots kept sorted. */
static void store(Hand *h, Card card) {
    for (int i = 0; i < h->count; ++i)
        if (!strcmp(h->cards[i].slot, card.slot)) { h->cards[i] = card; return; }
    if (h->count == MAX_HAND) return;
    int i = h->count++;
    while (i > 0 && strcmp(h->cards[i - 1].slot, card.slot) > 0) { h->cards[i] = h->cards[i - 1]; --i; }
    h->cards[i] = card;
}

static void apply(char events[][LINE_SIZE], int count) {
    char copy[LINE_SIZE], *parts[MAX_PARTS];
    int start = 0, last_new = NONE;
    for (int k = 0; k < count; ++k) {
        memcpy(copy, events[k], LINE_SIZE); /* both buffers are LINE_SIZE */
        int n = split(copy, parts);
        if (n >= 2 && !strcmp(parts[1], "NEWGAME") && is_digits(parts[0])) last_new = k;
    }
    if (last_new != NONE) {
        memcpy(copy, events[last_new], LINE_SIZE);
        split(copy, parts);
        reset(atoi(parts[0]));
        start = last_new + 1;
    }
    int player, tail = count;
    Card card;
    while (tail > start && visible_card(events[tail - 1], &player, &card)) --tail;
    hand_count = 0;
    for (int k = tail; k < count; ++k)
        if (visible_card(events[k], &player, &card)) store(hand_of(player, true), card);
    int hinted = NONE;
    for (int k = start; k < tail; ++k) {
        memcpy(copy, events[k], LINE_SIZE); /* both buffers are LINE_SIZE */
        int n = split(copy, parts);
        if (n < 2 || !is_digits(parts[0])) continue;
        int who = atoi(parts[0]);
        const char *kind = parts[1];
        if ((!strcmp(kind, "PLAY") || !strcmp(kind, "ERROR") || !strcmp(kind, "DISCARD")) && n >= 3) {
            if (!strcmp(kind, "PLAY") && n >= 4) {
                char colour[LINE_SIZE], level[LINE_SIZE];
                partition(parts[3], colour, level);
                int c = colour_index(colour);
                if (c != NONE && is_digits(level) && atoi(level) > fireworks[c]) fireworks[c] = atoi(level);
            }
            forget(who, parts[2]);
        } else if ((!strcmp(kind, "SAYLEVEL") || !strcmp(kind, "SAYCOLOR")) && n >= 3 && is_digits(parts[2])) {
            hinted = atoi(parts[2]);
        } else if (!strcmp(kind, "CARD") && n >= 4 && who == hinted) {
            learn(who, parts[2], parts[3]);
        }
    }
}

static bool all_colours(const char *slot, int level, bool useless) {
    Knowledge k = knowledge_of(me, slot);
    for (int c = 0; c < COLOR_COUNT; ++c) {
        if (k.colour != NONE && c != k.colour) continue;
        if (useless ? fireworks[c] < level : fireworks[c] != level - 1) return false;
    }
    return true;
}

static bool known_playable(const char *slot) {
    int level = knowledge_of(me, slot).level;
    return level != NONE && all_colours(slot, level, false);
}

static bool known_useless(const char *slot) {
    int level = knowledge_of(me, slot).level;
    return level != NONE && all_colours(slot, level, true);
}

static bool playable(const Card *c) { return fireworks[c->colour] == c->level - 1; }

/* Level then colour hint flagging only playable cards; never repeats an
 * attribute the player already knows (the boss plays a card it knows fully). */
static bool hint_for(int player, char *out) {
    Hand *h = hand_of(player, false);
    if (!h) return false;
    for (int i = 0; i < h->count; ++i) {
        const Card *card = &h->cards[i];
        Knowledge k = knowledge_of(player, card->slot);
        if (!playable(card) || (k.colour != NONE && k.level != NONE)) continue;
        for (int index = 1; index >= 0; --index) {
            if ((index == 1 ? k.level : k.colour) != NONE) continue;
            bool safe = true;
            for (int j = 0; j < h->count; ++j) {
                const Card *o = &h->cards[j];
                bool flagged = index == 1 ? o->level == card->level : o->colour == card->colour;
                if (flagged && !playable(o)) safe = false;
            }
            if (!safe) continue;
            if (index == 1) snprintf(out, LINE_SIZE, "SAY:%d:%d", player, card->level);
            else snprintf(out, LINE_SIZE, "SAY:%d:%s", player, COLORS[card->colour]);
            return true;
        }
    }
    return false;
}

static int teammate(int k) { return (me + k) % TEAM; }

static void harmless_hint(char *out) {
    for (int k = 1; k < TEAM; ++k) {
        Hand *h = hand_of(teammate(k), false);
        for (int i = 0; h && i < h->count; ++i)
            if (fireworks[h->cards[i].colour] >= h->cards[i].level) {
                snprintf(out, LINE_SIZE, "SAY:%d:%s", teammate(k), COLORS[h->cards[i].colour]);
                return;
            }
    }
    for (int k = 1; k < TEAM; ++k) {
        Hand *h = hand_of(teammate(k), false);
        if (h && h->count) {
            snprintf(out, LINE_SIZE, "SAY:%d:%d", teammate(k), h->cards[0].level);
            return;
        }
    }
    snprintf(out, LINE_SIZE, "SAY:%d:1", teammate(1));
}

static void decide(int info, char *out) {
    for (int i = 0; i < my_slot_count; ++i)
        if (known_playable(my_slots[i])) { snprintf(out, LINE_SIZE, "PLAY:%s", my_slots[i]); return; }
    if (info > 0)
        for (int k = 1; k < TEAM; ++k)
            if (hint_for(teammate(k), out)) return;
    if (info < MAX_INFO && my_slot_count > 0) {
        for (int i = 0; i < my_slot_count; ++i)
            if (known_useless(my_slots[i])) { snprintf(out, LINE_SIZE, "DISCARD:%s", my_slots[i]); return; }
        for (int i = 0; i < my_slot_count; ++i)
            if (find_known(me, my_slots[i]) == NONE) { snprintf(out, LINE_SIZE, "DISCARD:%s", my_slots[i]); return; }
        snprintf(out, LINE_SIZE, "DISCARD:%s", my_slots[0]);
        return;
    }
    harmless_hint(out);
}

static void strip(char *s) {
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ' || s[n - 1] == '\t')) s[--n] = '\0';
    size_t lead = strspn(s, " \t");
    memmove(s, s + lead, strlen(s + lead) + 1);
}

int main(void) {
    static char events[MAX_EVENTS][LINE_SIZE];
    char line[LINE_SIZE], out[LINE_SIZE];
    reset(0);
    while (fgets(line, sizeof line, stdin)) {
        int mistakes, info, count;
        if (sscanf(line, "%d %d", &mistakes, &info) != 2) return 0;
        if (!fgets(line, sizeof line, stdin) || sscanf(line, "%d", &count) != 1) return 0;
        if (count < 0) count = 0;
        if (count > MAX_EVENTS) count = MAX_EVENTS;
        for (int i = 0; i < count; ++i) {
            if (!fgets(events[i], LINE_SIZE, stdin)) events[i][0] = '\0';
            strip(events[i]);
        }
        apply(events, count);
        decide(info, out);
        printf("%s\n", out);
        fflush(stdout);
    }
    return 0;
}
