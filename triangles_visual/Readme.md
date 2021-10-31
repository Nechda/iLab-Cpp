# VTriangles
This repo consist code of second level task **triangles**, the main goal was implement simple program that allows to look at collection of trianges in real time.
# Installation
## Third-party libs
First of all you need install severall libs:
* glfw
* glm
* vulkan-sdk
## Windows
Here described step-by-step guide, how to compile the project.

**Use as command-line** `x86 Native Tools Command Prompt for VS 20xx` use **x86**, not x64, it's **important!**
```bash
    git clone https://github.com/Nechda/iLab-Cpp.git
    mkdir build
    cd build
    cmake -G "NMake Makefiles" ../iLab-Cpp/triangles_visual ^
        -DGLFW_DIR=<path_to_glfw> ^
        -DGLM_DIR=<path_to_glm> ^
        -DVULKAN_DIR=<path_to_vulkan>
    nmake
    triangles.exe < input.txt
```
## Linux
Here described step-by-step guide, how to compile the project, unlike windows you didn't have to set certain path to libs or choosing compilator.
```bash
    git clone https://github.com/Nechda/iLab-Cpp.git
    mkdir build
    cd build
    cmake -G Ninja ./iLab-Cpp/triangles_visual
    ninja
    ./triangles < input.txt
```
# Usage
## Internal represenation
After compilation you will get `triangles` executable file. To run the program you need pass the data into stdin in format described below:
```
<N_triangles>
<v[0].x> <v[0].y> <v[0].z>
<v[1].x> <v[1].y> <v[1].z>
<v[2].x> <v[2].y> <v[2].z>
<v[0].x> <v[0].y> <v[0].z>
<v[1].x> <v[1].y> <v[1].z>
<v[2].x> <v[2].y> <v[2].z>
...
<v[0].x> <v[0].y> <v[0].z>
<v[1].x> <v[1].y> <v[1].z>
<v[2].x> <v[2].y> <v[2].z>
```
The first line contain only one number --- total amount of triangles ```N_triangles```, next ```N_triangles x 3``` lines descibe coordinates of each triagle vertex.
One line --- one vertex, each three lines --- a new triangle.
## Helper
If you are so lazy and want to run ```triangles``` as soon as possible, I will make you happy: there is also ```gen.py``` that generate files, that you can pass into ```trianges```
program. Example of generating 347 trianges and writing its coordinates in `in` file.
```bash
python3 gen.py > in
347
./triangles < in
```
# Example
<p align="center">
  <img src="intersection.gif"/>
</p>
