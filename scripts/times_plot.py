from matplotlib import pyplot as plt
import numpy as np

times = []
sizes = [5, 10, 15, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250,
         275, 300, 325, 350, 375, 400, 425, 450, 475, 500]
for i in range(len(sizes)):
    times.append(float(input()))

sizes = np.array(sizes)
a, b, c = 0.000275, 0, 0

figure, ax = plt.subplots(1, figsize=(8, 8))
ax.plot(sizes, times, c="r", label="Real dependence")
ax.plot(sizes, a*sizes**2 + b*sizes + c, c="b", label=f"Approximation by $a*size^2$ (where $a \\approx {a})$")

ax.set_title("Time dependence on square matrix size")
ax.set_xlabel("Size")
ax.set_ylabel("Time")
ax.legend()
ax.grid(True)

plt.savefig("./Time_plot.png")
