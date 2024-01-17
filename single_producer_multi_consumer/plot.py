import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.stats import norm
from datetime import datetime
import numpy as np

csv_data = 'elapsed_times.csv'

df = pd.read_csv(csv_data) 
elapsed_time = df['Elapsed Time (ns)'] 

mean = elapsed_time.mean()
std_dev = elapsed_time.std()
median = elapsed_time.median()

print("Mediana:"+str(median)+" ns.")

title = f"Benchmarks SP - MC  Homogêneos\nMédia: {mean:.2f} ns, Desvio Padrão: {std_dev:.2f} ns, Mediana: {median:.2f} ns"

sns.set(style="whitegrid")
plt.figure(figsize=(10, 6))
sns.histplot(elapsed_time, bins=250, stat="density")
xmin, xmax = plt.xlim()
x = np.linspace(0, xmax, 500)
p = norm.pdf(x, mean, std_dev)
plt.plot(x, p,'k', linewidth=2)

current_time = datetime.now().strftime("%Y%m%d%H%M%S")

plt.title(title)
plt.show()
