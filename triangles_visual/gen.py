import random

def gen_triangle(x,y,z):
    origin = (x,y,z)
    for i in range(3):
        for j in range(3):
            p = random.uniform(-0.5, 0.5)
            print(origin[j] + p,end=" ")
        print()


N = int(input())
R = 100
print(N)
for i in range(N):
    x = random.uniform(-R/2, R/2)
    y = random.uniform(-R/2, R/2)
    z = random.uniform(-R/2, R/2)
    gen_triangle(x,y,z)

