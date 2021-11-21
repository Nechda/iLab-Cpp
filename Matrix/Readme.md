# Det-matrix
This repo consist code of det calculator for ineger matrices.
# Installation
## Third-party libs
First of all you need install **Boost** for supporting long numbers.
## Compilation
```bash
    git clone https://github.com/Nechda/iLab-Cpp.git
    mkdir build
    cd build
    cmake -G Ninja ../iLab-Cpp/Matrix
    ninja
```
# Usage
## Internal representation
After compilation you will get `matrix` executable file. For calculation determinant you need pass at the first line `N_COLS` in matrix, then at the next `N_COLS` lines
type `N_COLS` integer number for describing matrix. Example:
```bash
./matrix
2
0 7
4 0
```
# Testing
For running tests use commands:
```bash
python3 matrix_generator.py > data
./tester < data
```
