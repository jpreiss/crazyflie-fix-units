# Tool to resolve the symbolic expressions for controller gains into values.
# Instructions:
#   - gcc -E controller_mellinger.c > controller_mellinger_preprocessed.c
#   - Manually edit controller_mellinger_preprocessed.c to get rid of other headers
#   - cat controller_mellinger_preprocessed.c | python3 resolve_constants.py > controller_mellinger_resolved.c
#   - Manually use a diff tool to revert all the changes except the numerical constants

import re
import sys

regex = re.compile("static float .* = (.*);")

for line in sys.stdin:
    if line.startswith("#"):
        continue
    match = regex.match(line)
    if match:
        g = match.group(1)
        i, j = match.span(1)
        s = line[:i] + str(eval(g)) + "f" + line[j:]
        sys.stdout.write(s)
    else:
        sys.stdout.write(line)
