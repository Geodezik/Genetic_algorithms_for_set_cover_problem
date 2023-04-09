from matplotlib import pyplot as plt

files = ["C0001K50.txt", "C001K50.txt", "C01K50.txt", "C1K50.txt"]
labels = ["K=50, C=0.0001", "K=50, C=0.001", "K=50, C=0.01", "K=50, C=0.1", "K=50, C=1.0"]

figure, ax = plt.subplots(1, figsize=(8, 8))
for file, label in zip(files, labels):
    a = []
    with open(file, 'r') as f:
        for line in f:
            a.append(int(line))
    ax.plot(a, label=label)

ax.axhline(y=595, xmin=0, xmax=5000, color='black', linestyle='--', label="Жадный алгоритм")
ax.set_title("Сходимость ГА Сотнезова для одной задачи при различных параметрах C")
ax.set_xlabel("Итерация алгоритма")
ax.set_ylabel("Длина минимального покрытия")
ax.grid(True)
ax.legend()

figure.tight_layout()
plt.savefig("./Plot_C.png")
