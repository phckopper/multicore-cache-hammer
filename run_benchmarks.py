#!/usr/bin/env python3

from itertools import product
from subprocess import run
import os

iter_matrix_product = 100000000 # Estimated parameter to ensure ~1000ms runtime

# Uncomment to run on all core combinations
#cores = os.sched_getaffinity(0)

#cores = {0, 2, 8, 15}
core_pairs = [(0, 1), (0, 2), (0, 16), (0, 23), (16, 17), (16, 23), (20, 23)]
matrix_sizes = [1, 10, 100, 1000, 10000]
iters = [iter_matrix_product//m for m in matrix_sizes]


#for c1, c2 in product(cores, cores):
for c1, c2 in core_pairs:
    if c1 == c2:
        continue
    print("Now analyzing cores", c1, c2)
    for m_size, its in zip(matrix_sizes, iters):
        print(m_size, its)
        run(["./stress", str(c1), str(c2), str(m_size), str(its)])
