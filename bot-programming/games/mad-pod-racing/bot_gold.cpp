// Mad Pod Racing - Gold/Legend rolling-horizon genetic planner.
// Not submitted until the Gold protocol has been confirmed in the statement.
//
// Based on the public Legend solution by tcourreges and the CSB write-ups by
// Jeff06/pb4608. Each pod move is (rotation, thrust, shield, boost); a plan is
// four turns for both pods. The best plan is kept between turns, shifted by one
// turn, and improved with biased mutations until the time budget is exhausted.
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

constexpr int SIM_TURNS = 4;
constexpr int POP = 6;
constexpr int MAX_THRUST = 100;
constexpr int BOOST_THRUST = 650;
constexpr int MAX_ROT = 18;
constexpr int SHIELD_COOLDOWN = 4; // shield turn + 3
constexpr int FIRST_TURN_MS = 450;
constexpr int TURN_MS = 60;
constexpr float CP_R2 = 600.f * 600.f;
constexpr float POD_R2 = 400.f * 400.f;
constexpr float MIN_IMPULSE = 120.f;
constexpr float FRICTION = 0.85f;
constexpr float PI = 3.14159265f;
constexpr float SAFETY = 0.90f;

struct Vec {
    float x = 0.f, y = 0.f;
    Vec() = default;
    Vec(float x_, float y_) : x(x_), y(y_) {}
};
Vec operator+(Vec a, Vec b) { return Vec(a.x + b.x, a.y + b.y); }
Vec operator-(Vec a, Vec b) { return Vec(a.x - b.x, a.y - b.y); }
Vec operator*(float k, Vec a) { return Vec(k * a.x, k * a.y); }
Vec operator*(Vec a, float k) { return Vec(k * a.x, k * a.y); }
Vec& operator+=(Vec& a, Vec b) { a = a + b; return a; }
Vec& operator*=(Vec& a, float k) { a = a * k; return a; }
float dot(Vec a, Vec b) { return a.x * b.x + a.y * b.y; }
float dist2(Vec a, Vec b) { return dot(a - b, a - b); }
float dist(Vec a, Vec b) { return sqrt(dist2(a, b)); }
float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
float normDeg(float a) {
    while (a > 180.f) a -= 360.f;
    while (a < -180.f) a += 360.f;
    return a;
}

struct Pod {
    Vec pos, speed;
    int angle = 0;
    int cpId = 0;
    int passed = 0;
    bool canBoost = true;
    int shieldCd = 0;
};

void manageShield(bool on, Pod& p) {
    if (on) p.shieldCd = SHIELD_COOLDOWN;
    else if (p.shieldCd > 0) --p.shieldCd;
}
int mass(const Pod& p) { return p.shieldCd == SHIELD_COOLDOWN ? 10 : 1; }

float collisionTime(const Pod& a, const Pod& b) {
    const Vec dp = b.pos - a.pos;
    const Vec ds = b.speed - a.speed;
    const float aa = dot(ds, ds);
    if (aa < 1e-6f) return numeric_limits<float>::infinity();
    const float bb = -2.f * dot(dp, ds);
    const float cc = dot(dp, dp) - 4.f * POD_R2;
    const float delta = bb * bb - 4.f * aa * cc;
    if (delta < 0.f) return numeric_limits<float>::infinity();
    const float t = (bb - sqrt(delta)) / (2.f * aa);
    if (t <= 1e-6f) return numeric_limits<float>::infinity();
    return t;
}

void rebound(Pod& a, Pod& b) {
    const int ma = mass(a), mb = mass(b);
    const Vec dp = b.pos - a.pos;
    const float ab = dist(a.pos, b.pos);
    if (ab < 1e-6f) return;
    const Vec u = (1.f / ab) * dp;
    const Vec ds = b.speed - a.speed;
    const float m = float(ma * mb) / float(ma + mb);
    float impulse = -2.f * m * dot(ds, u);
    // CSB adds a *minimum* impulse of 120: a weak rebound is raised to 120 in
    // the direction it already points, never reduced to it.
    if (fabsf(impulse) < MIN_IMPULSE)
        impulse = impulse < 0.f ? -MIN_IMPULSE : MIN_IMPULSE;
    a.speed += (-1.f / ma) * impulse * u;
    b.speed += (1.f / mb) * impulse * u;
}

struct Move {
    int rot = 0;
    int thrust = 0;
    bool shield = false;
    bool boost = false;
};
using Turn = array<Move, 2>;
struct Solution {
    array<Turn, SIM_TURNS> turns{};
    float score = -numeric_limits<float>::infinity();
};

struct Sim {
    vector<Vec> cps;
    int cpCount = 0, maxPassed = 0;

    void readInit() {
        int laps = 0;
        cin >> laps >> cpCount;
        cps.reserve(cpCount);
        for (int i = 0; i < cpCount; ++i) {
            int x, y;
            cin >> x >> y;
            cps.emplace_back(float(x), float(y));
        }
        maxPassed = laps * cpCount;
    }

    void play(vector<Pod>& pods, const Solution& s) const {
        for (const Turn& turn : s.turns) {
            for (int i = 0; i < 2; ++i) {
                Pod& p = pods[i];
                const Move& m = turn[i];
                p.angle = (p.angle + m.rot) % 360;
                if (p.angle < 0) p.angle += 360;
                manageShield(m.shield, p);
                if (p.shieldCd > 0) continue;
                const float r = p.angle * PI / 180.f;
                const bool boost = p.canBoost && m.boost;
                const int power = boost ? BOOST_THRUST : m.thrust;
                if (boost) p.canBoost = false;
                p.speed += power * Vec{cosf(r), sinf(r)};
            }

            float t = 0.f;
            while (t < 1.f) {
                float dt = 1.f - t;
                int ca = -1, cb = -1;
                for (int i = 0; i < 4; ++i) {
                    for (int j = i + 1; j < 4; ++j) {
                        const float ct = collisionTime(pods[i], pods[j]);
                        if (ct < dt) { dt = ct; ca = i; cb = j; }
                    }
                }
                for (Pod& p : pods) {
                    p.pos += dt * p.speed;
                    if (dist2(p.pos, cps[p.cpId]) < CP_R2) {
                        p.cpId = (p.cpId + 1) % cpCount;
                        ++p.passed;
                    }
                }
                if (ca >= 0) rebound(pods[ca], pods[cb]);
                t += dt;
                if (dt <= 1e-6f) break;
            }

            for (Pod& p : pods) {
                p.speed *= FRICTION;
                p.speed = Vec{float(int(p.speed.x)), float(int(p.speed.y))};
                p.pos = Vec{roundf(p.pos.x), roundf(p.pos.y)};
            }
        }
    }

    float rate(const vector<Pod>& pods) const {
        auto score = [&](const Pod& p) {
            const float d = dist(p.pos, cps[p.cpId]);
            return 30000.f * float(p.passed) - d;
        };
        const float s0 = score(pods[0]), s1 = score(pods[1]);
        const int myRacer = s0 > s1 ? 0 : 1;
        const Pod& racer = pods[myRacer];
        const Pod& blocker = pods[1 - myRacer];
        const float os0 = score(pods[2]), os1 = score(pods[3]);
        const Pod& oppRacer = os0 > os1 ? pods[2] : pods[3];
        if (racer.passed >= maxPassed) return numeric_limits<float>::infinity();
        if (oppRacer.passed >= maxPassed) return -numeric_limits<float>::infinity();
        // Sum of both own pods minus both opponents, plus a light blocker term.
        // Local 4-pod harness beat the strict racer/blocker formula with this.
        const float ahead = s0 + s1 - os0 - os1;
        const float block = -dist(blocker.pos, cps[oppRacer.cpId]);
        return 2.f * ahead + 0.5f * block;
    }
};

unsigned int rngState = 12345u;
int rnd(int a, int b) {
    rngState = 214013u * rngState + 2531011u;
    int v = int((rngState >> 16) & 0x7fffu);
    return a + v % (b - a);
}

Vec heuristicTarget(const Pod& p, const vector<Vec>& cps, bool racer) {
    if (!racer) {
        return cps[p.cpId];
    }
    const Vec cp = cps[p.cpId];
    return cp - 3.f * p.speed;
}

int heuristicRotation(const Pod& p, Vec target) {
    const Vec d = target - p.pos;
    float want = atan2f(d.y, d.x) * 180.f / PI;
    if (want < 0.f) want += 360.f;
    return clampi(int(roundf(normDeg(want - float(p.angle)))), -MAX_ROT, MAX_ROT);
}

int heuristicThrust(const Pod& p, const vector<Vec>& cps) {
    const Vec cp = cps[p.cpId];
    const Vec to = cp - p.pos;
    float want = atan2f(to.y, to.x) * 180.f / PI;
    if (want < 0.f) want += 360.f;
    const float err = fabsf(normDeg(want - float(p.angle)));
    const float d = dist(cp, p.pos);
    return int(100.f * clampf(1.f - err / 90.f, 0.f, 1.f)
               * clampf(d / 1200.f, 0.f, 1.f));
}

void randomize(Move& m, bool all) {
    const int choice = all ? -1 : rnd(0, 11);
    const bool doRot = all || choice <= 4;
    const bool doThrust = all || (choice > 4 && choice <= 9);
    const bool doShield = all || choice == 10;
    if (doRot) {
        const int r = rnd(-2 * MAX_ROT, 3 * MAX_ROT);
        m.rot = r > 2 * MAX_ROT ? 0 : clampi(r, -MAX_ROT, MAX_ROT);
    }
    if (doThrust) m.thrust = clampi(rnd(-50, 200), 0, MAX_THRUST);
    if (doShield) m.shield = !m.shield;
}

void shift(Solution& s) {
    for (int t = 1; t < SIM_TURNS; ++t) s.turns[t - 1] = s.turns[t];
    for (int i = 0; i < 2; ++i) randomize(s.turns[SIM_TURNS - 1][i], true);
}

void seedHeuristic(Solution& s, const vector<Pod>& pods, const vector<Vec>& cps) {
    for (int i = 0; i < 2; ++i) {
        Move& m = s.turns[0][i];
        m.rot = heuristicRotation(pods[i], heuristicTarget(pods[i], cps, i == 0));
        m.thrust = heuristicThrust(pods[i], cps);
        m.shield = false;
        m.boost = false;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Sim sim;
    sim.readInit();

    vector<Solution> pool(2 * POP);
    for (int s = 0; s < POP; ++s)
        for (int t = 0; t < SIM_TURNS; ++t)
            for (int i = 0; i < 2; ++i)
                randomize(pool[s].turns[t][i], true);

    // If the first stretch is long, boost on turn 1 rather than wasting search.
    if (sim.cpCount > 1 && dist2(sim.cps[0], sim.cps[1]) > 9000000.f)
        // The boost is shared by both pods in Gold/Legend: give it to one only.
        for (int s = 0; s < POP; ++s)
            pool[s].turns[0][0].boost = true;

    vector<Pod> pods(4);
    int step = 0;
    while (true) {
        for (int i = 0; i < 4; ++i) {
            int x, y, vx, vy, angle, cpId;
            cin >> x >> y >> vx >> vy >> angle >> cpId;
            Pod& p = pods[i];
            p.pos = Vec(float(x), float(y));
            p.speed = Vec(float(vx), float(vy));
            p.angle = angle;
            if (p.cpId != cpId) ++p.passed;
            p.cpId = cpId;
            if (step == 0) {
                p.canBoost = true;
                p.shieldCd = 0;
                const Vec target = sim.cps[cpId];
                const Vec d = target - p.pos;
                float a = atan2f(d.y, d.x) * 180.f / PI;
                if (a < 0.f) a += 360.f;
                p.angle = int(roundf(a)) % 360;
            }
        }

        const int budget = step == 0 ? FIRST_TURN_MS : TURN_MS;
        const auto start = chrono::steady_clock::now();
        auto timeLeft = [&]() {
            const auto ms = chrono::duration_cast<chrono::milliseconds>(
                chrono::steady_clock::now() - start).count();
            return ms < int(budget * SAFETY);
        };

        for (int s = 0; s < POP; ++s) {
            if (s == 0) seedHeuristic(pool[s], pods, sim.cps);
            else shift(pool[s]);
            vector<Pod> copy = pods;
            sim.play(copy, pool[s]);
            pool[s].score = sim.rate(copy);
        }

        while (timeLeft()) {
            for (int s = 0; s < POP; ++s) {
                Solution& child = pool[POP + s] = pool[s];
                const int k = rnd(0, 2 * SIM_TURNS);
                randomize(child.turns[k / 2][k % 2], false);
                vector<Pod> copy = pods;
                sim.play(copy, child);
                child.score = sim.rate(copy);
            }
            sort(pool.begin(), pool.end(), [](const Solution& a, const Solution& b) {
                return a.score > b.score;
            });
        }

        const Solution& best = pool[0];
        for (int i = 0; i < 2; ++i) {
            const Move& m = best.turns[0][i];
            const int a = (pods[i].angle + m.rot + 360) % 360;
            const float r = a * PI / 180.f;
            const Vec target = pods[i].pos + 10000.f * Vec{cosf(r), sinf(r)};
            cout << roundf(target.x) << ' ' << roundf(target.y) << ' ';
            if (m.shield) cout << "SHIELD";
            else if (m.boost) cout << "BOOST";
            else cout << m.thrust;
            cout << '\n';

            manageShield(m.shield, pods[i]);
            if (pods[i].shieldCd == 0 && m.boost) pods[i].canBoost = false;
        }
        cout.flush();
        ++step;
    }
}
