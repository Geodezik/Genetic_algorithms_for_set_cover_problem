from matplotlib import pyplot as plt

a = []
n=100
for i in range(100):
    a.append(int(input()))

figure, ax = plt.subplots(1, figsize=(8, 8))
ax.plot(a)
ax.scatter(n, a[-1], c='r', linewidth=2.5, marker="*", label=f"Coverage length = {a[-1]}")

ax.set_title("Coverage genetic algorithm progress")
ax.set_xlabel("Generation")
ax.set_ylabel("Elements in coverage")
ax.grid(True)
ax.legend()

plt.savefig("./Plot.png")
