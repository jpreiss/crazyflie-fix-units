import sys

import numpy as np

original_outputs = np.genfromtxt('original_outputs.txt', delimiter=',')
print(original_outputs)
new_outputs = np.genfromtxt('new_outputs.txt', delimiter=',')
print(new_outputs)

diff = np.abs(original_outputs - new_outputs)
diff_pct = 100.0 * diff / (2  ** 16)
maxdiff = np.max(diff.flat)
maxdiff_pct = np.max(diff_pct.flat)

TOLERANCE_PCT = 1

print(f"Maximum difference: {maxdiff_pct:.2f}%")
if maxdiff_pct > TOLERANCE_PCT:
    print("Error: Difference tolerance exceeded.")
    print(f"Tolerance exceeded on {np.sum(diff_pct > TOLERANCE_PCT)} out of {diff.size} outputs")
    sys.exit(1)
else:
    print("OK")
    sys.exit(0)
