a = []
for i in range(5000):
    x = input()
    a.append(x)
with open("C1K50.txt", 'w') as f:
    for elem in a:
        f.write(elem + '\n')
a = []
with open("C1K50.txt", 'r') as f:
    for line in f:
        a.append(int(line))
print(len(a))
