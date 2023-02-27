from time import time
import subprocess

# 5 10 15 25 50 75 100 125 150 175 200 225 250
start = time()
subprocess.run(["./test"], shell=True)
end = time()

print(end - start)
