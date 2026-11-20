#!/usr/bin/env python3
"""Summarize a Mad Pod Racing test log: frames, legs, result."""
import sys
import re

path = sys.argv[1]
text = open(path).read()
frames = len(re.findall(r"^frame \d+", text, re.M))
res = re.search(r"result\n((?:.|\n)*)$", text)
legs = {}
for m in re.finditer(r"^frame (\d+) agent (\d+)\n(?:  .*\n)*?  referee: Pod 1 of player \$(\d+) (?:moves towards|boosts towards) \(([-\d]+), ([-\d]+)\)", text, re.M):
    a = int(m.group(2))
    legs.setdefault(a, []).append((int(m.group(4)), int(m.group(5))))
for a in sorted(legs):
    seq = legs[a]
    changes = 1 + sum(1 for i in range(1, len(seq)) if seq[i] != seq[i - 1])
    print("agent %d: %d legs, %d actions" % (a, changes, len(seq)))
print("frames:", frames)
if res:
    print(res.group(1).strip())
