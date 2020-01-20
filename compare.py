import sys

import numpy as np

original_outputs = np.genfromtxt('original_outputs.txt', delimiter=',')
print(original_outputs)
new_outputs = np.genfromtxt('new_outputs.txt', delimiter=',')
print(new_outputs)

diff = np.abs(original_outputs - new_outputs)
maxdiff = np.max(diff.flat)
maxdiff_pct = 100 * maxdiff / (2 ** 16)

TOLERANCE_PCT = 1

print(f"Maximum difference: {maxdiff_pct:.2f}%")
if maxdiff_pct > TOLERANCE_PCT:
    print("Error: Difference tolerance exceeded.")
    sys.exit(1)
else:
    print("OK")
    sys.exit(0)
