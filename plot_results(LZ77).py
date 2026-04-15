import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

data = pd.read_csv('huffman_comparison.csv')

fig, ax1 = plt.subplots(figsize=(10, 6))

# Primary Axis: Compression Ratio
ax1.set_xlabel('Window Size (Bytes)')
ax1.set_ylabel('Compression Ratio (%)', color='tab:blue')
line1 = ax1.plot(data['WindowSize'], data['FusedRatio'], color='tab:blue', marker='o', label='Compression Ratio')
ax1.tick_params(axis='y', labelcolor='tab:blue')

# Secondary Axis: Time Trend
ax2 = ax1.twinx()
ax2.set_ylabel('Execution Time Trend (ms)', color='tab:red')

# Calculate Linear Trend Line for Time
z = np.polyfit(data['WindowSize'], data['FusedTime'], 1)
p = np.poly1d(z)
line2 = ax2.plot(data['WindowSize'], p(data['WindowSize']), color='tab:red', linestyle='--', linewidth=2, label='Time Complexity (Trend)')
ax2.tick_params(axis='y', labelcolor='tab:red')

# Combined Legend
lns = line1 + line2
labs = [l.get_label() for l in lns]
ax1.legend(lns, labs, loc='upper left', frameon=True, shadow=True, title="LZ77 Metrics")

plt.title('LZ77 Analysis: Window Size vs Efficiency & Time Trend')
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()