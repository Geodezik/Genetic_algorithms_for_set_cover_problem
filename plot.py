from matplotlib import pyplot as plt

a = []
for i in range(100):
    a.append(int(input()))

figure, ax = plt.subplots(1, figsize=(8, 8))
ax.plot(a)
plt.savefig("./Plot.png")
