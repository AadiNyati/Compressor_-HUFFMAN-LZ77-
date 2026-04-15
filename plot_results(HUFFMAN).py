import pandas as pd
import matplotlib.pyplot as plt

# Load the data
data = pd.read_csv('huffman_comparison.csv')

plt.figure(figsize=(10, 6))

# Plot the two ratios
plt.plot(data['WindowSize'], data['FusedRatio'], color='firebrick', marker='o', linewidth=2, label='Fused Huffman Ratio')
plt.plot(data['WindowSize'], data['SplitRatio'], color='forestgreen', marker='s', linewidth=2, label='Split Huffman Ratio')

# Formatting
plt.title('Huffman Strategy Comparison: Encoding Efficiency', fontsize=14)
plt.xlabel('Window Size (Bytes)', fontweight='bold')
plt.ylabel('Compression Ratio (%)', fontweight='bold')

# Legend Table
plt.legend(loc='best', frameon=True, shadow=True, title="Encoding Strategy")

# Visual Aids
plt.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()
plt.show()