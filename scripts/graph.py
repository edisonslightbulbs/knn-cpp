#!/usr/bin/env python

import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
import os
from matplotlib.colors import ListedColormap

pwd = os.getcwd()

data_file = pwd + "/build/output_data.txt"
plt.figure()
df = pd.read_csv(data_file)

sns.lineplot(x=df.id, y=df.nn1, hue = 100)

plt.xlabel("id")
plt.ylabel("Euclidean dist")
plt.title("knn")

plt.show()
