// Start-up! — Rust port of cpp/bot.cpp (same economy, same tie-breaks).
// Build: cargo build --release
use std::io::{self, Write};

const SELL_FEATURES: i64 = 10; // sales need at least 10 features
const DEV_TARGET: i64 = 12;
const DEVELOP_EVERY: i64 = 6; // once selling, 1 developer in 6 adds features
const EMPLOYEES_PER_MANAGER: i64 = 6; // past 4 some misbehave; speed is worth it
const HIRES_PER_MANAGER: i64 = 2;
const MAX_MANAGERS: i64 = 10;
const EMPLOYEE_PAY: i64 = 10;
const MANAGER_PAY: i64 = 20;
const SAFETY_TURNS: i64 = 3; // payrolls kept in cash (income counted)
const FULL_SHARE: i64 = 1000; // market share is in thousandths
const UNFILLED_PER_PLAYER: i64 = 30; // 3 % of the market per player each turn

struct State { id: i64, income: i64, cash: i64, devs: i64, sellers: i64, managers: i64, features: i64, bugs: i64 }

#[derive(Clone, Copy)]
struct Player { id: i64, share: i64, reputation: i64 }

fn payroll(employees: i64, managers: i64) -> i64 { EMPLOYEE_PAY * employees + MANAGER_PAY * managers }

fn affordable(s: &State, employees: i64, managers: i64) -> bool {
    s.cash + s.income * SAFETY_TURNS >= payroll(employees, managers) * SAFETY_TURNS
}

/// (devs to hire, sellers to hire, managers to hire).
fn staffing(s: &State, ready: bool) -> (i64, i64, i64) {
    let (employees, managers) = (s.devs + s.sellers, s.managers);
    let cap = (HIRES_PER_MANAGER * managers).min(EMPLOYEES_PER_MANAGER * managers - employees).max(0);
    let mut dev_hire = cap.min((DEV_TARGET - s.devs).max(0));
    let mut sell_hire = if ready { cap - dev_hire } else { 0 };
    while dev_hire + sell_hire > 0 && !affordable(s, employees + dev_hire + sell_hire, managers) {
        if sell_hire > 0 { sell_hire -= 1; } else { dev_hire -= 1; }
    }
    let staff = employees + dev_hire + sell_hire;
    let hire_manager = managers < MAX_MANAGERS && affordable(s, staff, managers + 1);
    (dev_hire, sell_hire, hire_manager as i64)
}

/// Developers assigned to fixing bugs and writing tests.
fn maintenance(s: &State, devs: i64) -> i64 {
    if s.features < SELL_FEATURES { return devs / 2; }
    devs - (devs / DEVELOP_EVERY).min(devs - s.bugs).max(0)
}

/// (competitive sellers, focused rival id).
fn fight(me: &Player, players: &[Player], sellers: i64) -> (i64, Option<i64>) {
    let shares: i64 = players.iter().map(|p| p.share).sum();
    let rivals = players.iter().filter(|p| p.id != me.id).count();
    let eligible: Vec<Player> =
        players.iter().filter(|p| p.id != me.id && p.share > 0 && p.reputation <= me.reputation).copied().collect();
    if eligible.is_empty() || FULL_SHARE - shares >= UNFILLED_PER_PLAYER * players.len() as i64 { return (0, None); }
    let mut target = None;
    if eligible.len() < rivals {
        let mut best = eligible[0];
        for p in &eligible[1..] {
            if p.share > best.share { best = *p; }
        }
        target = Some(best.id);
    }
    (sellers / 2, target)
}

fn decide(s: &State, players: &[Player]) -> String {
    let me = players.iter().find(|p| p.id == s.id).copied().unwrap_or(Player { id: s.id, share: 0, reputation: 0 });
    let (dev_hire, sell_hire, manager_hire) = staffing(s, s.features >= SELL_FEATURES);
    let (devs, sellers) = (s.devs + dev_hire, s.sellers + sell_hire);
    let (competitive, target) = fight(&me, players, sellers);
    let mut line = format!("{} {} {} {} {}", dev_hire, sell_hire, manager_hire, maintenance(s, devs), competitive);
    if let Some(t) = target { line += &format!(" {}", t); }
    line
}

/// Integers as they arrive, line by line (reading all stdin at once would block the game).
fn tokens() -> impl Iterator<Item = i64> {
    io::stdin()
        .lines()
        .map_while(Result::ok)
        .flat_map(|line| line.split_ascii_whitespace().map(|t| t.parse::<i64>().unwrap_or(0)).collect::<Vec<_>>())
}

fn main() {
    let mut tokens = tokens();
    let stdout = io::stdout();
    let mut out = stdout.lock();
    loop {
        let head: Vec<i64> = tokens.by_ref().take(11).collect();
        if head.len() < 11 { return; }
        let s = State {
            id: head[0], income: head[3], cash: head[4], devs: head[5], sellers: head[6],
            managers: head[7], features: head[8], bugs: head[10],
        };
        let count = head[1].max(0) as usize;
        let mut players = Vec::with_capacity(count);
        for _ in 0..count {
            let p: Vec<i64> = tokens.by_ref().take(3).collect();
            if p.len() < 3 { return; }
            players.push(Player { id: p[0], share: p[1], reputation: p[2] });
        }
        writeln!(out, "{}", decide(&s, &players)).unwrap();
        out.flush().unwrap();
    }
}
