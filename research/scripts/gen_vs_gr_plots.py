from matplotlib import pyplot as plt
import numpy as np

f = open("results_gen.txt", 'r')
gen = [int(elem) for elem in f.read().strip().split()]
gen = np.array(gen).reshape(-1, 2)
f.close()

f = open("results_gr.txt", 'r')
gr = [int(elem) for elem in f.read().strip().split()]
gr = np.array(gr).reshape(-1, 2)
f.close()

sizes = [i * 1000 for i in range(1, 10 + 1)]

figure, ax = plt.subplots(1, figsize=(6, 6))
ax.plot(sizes, gen[:, 0], label="Genetic")
ax.plot(sizes[:6], gr[:, 0], label="Greedy")

ax.set_title("Time dependence on square matrix size")
ax.set_xlabel("Size")
ax.set_ylabel("Time (s)")
ax.legend()
ax.grid(True)

plt.savefig("./Time_plot.png")