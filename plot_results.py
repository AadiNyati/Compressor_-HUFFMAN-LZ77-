import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

try:
    data = pd.read_csv('window_sensitivity_averaged.csv')
except FileNotFoundError:
    print("Error: 'window_sensitivity_averaged.csv' not found. Run your C++ code first!")
    exit()

fig, ax1 = plt.subplots(figsize=(12, 7))

color_ratio = 'tab:blue'
ax1.set_xlabel('Window Size (Bytes)', fontsize=12, fontweight='bold')
ax1.set_ylabel('Compression Ratio (%)', color=color_ratio, fontsize=12, fontweight='bold')
ax1.plot(data['WindowSize'], data['CompressionRatio'], color=color_ratio, marker='o', alpha=0.4, label='Raw Ratio')

z_ratio = np.polyfit(data['WindowSize'], data['CompressionRatio'], 3)
p_ratio = np.poly1d(z_ratio)
ax1.plot(data['WindowSize'], p_ratio(data['WindowSize']), color=color_ratio, linewidth=2, label='Ratio Trend')

ax1.tick_params(axis='y', labelcolor=color_ratio)
ax1.grid(True, linestyle='--', alpha=0.5)

ax2 = ax1.twinx() 
color_time = 'tab:red'
ax2.set_ylabel('Average Execution Time (ms)', color=color_time, fontsize=12, fontweight='bold')
ax2.plot(data['WindowSize'], data['AverageTimeMS'], color=color_time, marker='s', alpha=0.4, label='Raw Avg Time')

z_time = np.polyfit(data['WindowSize'], data['AverageTimeMS'], 1)
p_time = np.poly1d(z_time)
ax2.plot(data['WindowSize'], p_time(data['WindowSize']), color=color_time, linewidth=2, linestyle='--', label='Time Trend')

ax2.tick_params(axis='y', labelcolor=color_time)

plt.title('Averaged Sensitivity Analysis: LZ77-Huffman Hybrid', fontsize=14, pad=20)
fig.tight_layout()

lines, labels = ax1.get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()
ax1.legend(lines + lines2, labels + labels2, loc='upper left')

plt.savefig('averaged_research_plot.png', dpi=300)
print("Success! Plot saved as 'averaged_research_plot.png'")
plt.show()