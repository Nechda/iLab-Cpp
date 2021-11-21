import numpy as np

def gen_matrix(N = 3):
    mat = np.random.randint(low = -25, high = 25, size = (N, N), dtype = np.int64)
    return mat

N_tests = 100
Matrix_size = 5

print(N_tests)
print(Matrix_size)

for i in range(N_tests):
    mat = gen_matrix(Matrix_size)
    det = np.round(np.linalg.det(mat),1)

    for it in mat:
        print(*it)
    print(int(det))