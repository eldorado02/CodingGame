#!/usr/bin/env python3
"""Small CodinGame API client used by every bot in ../games/<game>/.

Token: ~/.config/codingame/token (never printed, never stored in the repo).
Docs:  https://www.codingame.com/api/v1/docs/api

Examples
    ./cg.py list --category bot-programming
    ./cg.py show tic-tac-toe
    ./cg.py stub tic-tac-toe
    ./cg.py test tic-tac-toe ../games/tic-tac-toe/bot.py
    ./cg.py submit tic-tac-toe ../games/tic-tac-toe/bot.py
    ./cg.py progress tic-tac-toe
"""

import argparse
import json
import os
import sys
import time
import urllib.error
import urllib.parse
import urllib.request

API = "https://www.codingame.com/api/v1"
TOKEN_PATH = os.path.expanduser("~/.config/codingame/token")
HERE = os.path.dirname(os.path.abspath(__file__))


def token():
    try:
        with open(TOKEN_PATH) as f:
            return f.read().strip()
    except FileNotFoundError:
        sys.exit("No token in %s. Run the device flow from the skill first." % TOKEN_PATH)


def request(method, path, body=None, raw=False):
    url = path if path.startswith("http") else API + path
    data = None
    headers = {"Authorization": "Bearer " + token()}
    if body is not None:
        data = json.dumps(body).encode()
        headers["Content-Type"] = "application/json"
    req = urllib.request.Request(url, data=data, headers=headers, method=method)
    try:
        for attempt in range(20):  # retry DNS / network blips, not HTTP errors
            try:
                with urllib.request.urlopen(req) as resp:
                    payload = resp.read().decode()
                break
            except urllib.error.HTTPError:
                raise
            except (urllib.error.URLError, OSError) as e:
                sys.stderr.write("network error %s, retry in 15s\n" % e)
                time.sleep(15)
        else:
            sys.exit("network unavailable")
    except urllib.error.HTTPError as e:
        payload = e.read().decode()
        sys.stderr.write("HTTP %s\n%s\n" % (e.code, payload))
        if e.headers.get("Retry-After"):
            sys.stderr.write("Retry-After: %s\n" % e.headers["Retry-After"])
        sys.exit(1)
    return payload if raw else json.loads(payload)


def wait_for_test(resp):
    """Follow a 202 test game until it is done."""
    while resp.get("status") not in ("done", "failed", None):
        tid = resp.get("test_game_id")
        if not tid:
            break
        time.sleep(2)
        resp = request("GET", "/test-games/%s?wait=30" % tid)
    return resp


def dump_log(resp, game_id, tag="test"):
    """Print the text log and save it next to the game's bot."""
    log = resp.get("log")
    if log:
        print(log)
        d = os.path.join(HERE, "..", "games", game_id, "logs")
        os.makedirs(d, exist_ok=True)
        name = "%s-%s.log" % (tag, time.strftime("%Y%m%d-%H%M%S"))
        with open(os.path.join(d, name), "w") as f:
            f.write(log)
        sys.stderr.write("log saved: %s\n" % os.path.normpath(os.path.join(d, name)))
    else:
        print(json.dumps(resp, indent=2))


def cmd_list(args):
    q = {"arcade": "false", "limit": "100"}
    if args.category:
        q["category"] = args.category
    if args.played:
        q["played"] = "true"
    if args.search:
        q["search"] = args.search
    data = request("GET", "/puzzles?" + urllib.parse.urlencode(q))
    for p in data["puzzles"]:
        print("%-45s %-55s %5s %s" % (
            p["pretty_id"], p["title"], p.get("rating") or "-",
            "PLAYED" if p.get("played") else ""))
    print("\n%d games" % len(data["puzzles"]), file=sys.stderr)


def cmd_show(args):
    data = request("GET", "/puzzles/" + args.game)
    print(data.get("statement") or json.dumps(data, indent=2))
    print("\n-- languages: %s" % ", ".join(
        l["id"] for l in data.get("languages", [])), file=sys.stderr)


def cmd_stub(args):
    print(json.dumps(request("GET", "/puzzles/%s/stub-generator" % args.game), indent=2))


def cmd_opponents(args):
    print(json.dumps(request("GET", "/puzzles/%s/opponents" % args.game), indent=2))


def cmd_test(args):
    body = {"language": args.language, "code": open(args.bot).read()}
    if args.testcase is not None:
        body["testcase"] = args.testcase
    else:
        body["opponents"] = args.opponents.split(",")
    body.update(json.loads(args.extra) if args.extra else {})
    resp = request("POST", "/puzzles/%s/test-games?wait=30" % args.game, body)
    resp = wait_for_test(resp)
    dump_log(resp, args.game)


def cmd_submit(args):
    body = {"language": args.language, "code": open(args.bot).read()}
    resp = request("POST", "/puzzles/%s/submissions" % args.game, body)
    print(json.dumps(resp, indent=2))


def cmd_get(args):
    print(json.dumps(request("GET", "/puzzles/%s/%s" % (args.game, args.what)
                             if args.what else "/puzzles/" + args.game), indent=2))


def main():
    p = argparse.ArgumentParser(description="CodinGame bot programming client")
    sub = p.add_subparsers(dest="cmd", required=True)

    s = sub.add_parser("list", help="list bot programming / optimization / code golf puzzles")
    s.add_argument("--category", default=None)
    s.add_argument("--search", default=None)
    s.add_argument("--played", action="store_true")
    s.set_defaults(fn=cmd_list)

    for name, fn, hlp in (("show", cmd_show, "print the statement (Markdown)"),
                          ("stub", cmd_stub, "print the input/output format"),
                          ("opponents", cmd_opponents, "list available opponents")):
        s = sub.add_parser(name, help=hlp)
        s.add_argument("game")
        s.set_defaults(fn=fn)

    s = sub.add_parser("get", help="raw JSON of a puzzle sub-resource")
    s.add_argument("game")
    s.add_argument("what", nargs="?", choices=["progress", "battles", "leaderboard",
                                               "submissions", "stats", "opponents"])
    s.set_defaults(fn=cmd_get)

    s = sub.add_parser("test", help="run a test game")
    s.add_argument("game")
    s.add_argument("bot")
    s.add_argument("--language", default="Python3")
    s.add_argument("--opponents", default="default")
    s.add_argument("--testcase", type=int, default=None)
    s.add_argument("--extra", default=None, help="extra JSON fields, e.g. '{\"game_options\":{}}'")
    s.set_defaults(fn=cmd_test)

    s = sub.add_parser("submit", help="submit the bot")
    s.add_argument("game")
    s.add_argument("bot")
    s.add_argument("--language", default="Python3")
    s.set_defaults(fn=cmd_submit)

    args = p.parse_args()
    args.fn(args)


if __name__ == "__main__":
    main()
