# PinocchioPlus

This repository is conducting a research on the paper "Automatic Rigging and animation of 3D characters".

## 1 Installation

### 1.1 Ubuntu 18.04 (Prefered)

dependencies:
- `sudo apt-get install libfltk1.3-dev`
- `sudo apt-get install libxrender-dev libxcursor-dev libxft-dev libfontconfig-dev libxinerama-dev`

#### 1.1.1 Option 1: cmake

PinocchioJingma([jingma-git/Pinocchio](https://github.com/jingma-git/Pinocchio)) and PinocchioPlus are seperated projects.

```
# In the PinocchioJingma directory
mkdir build
cd build
cmake ..
make
```

Example:

```
./DemoUI ../../data/FinalBaseMesh.obj -motion ../../data/legacy/walk.txt
```

```
# In the PinocchioPlus directory
mkdir build
cd build
cmake ..
make
```

Example:

```
./PinocchioPlusDemoUI ../../data/FinalBaseMesh.obj -motion ../../data/legacy/walk.txt
```

Targets will be in the build directory.

### 1.2 Windows (Visual Studio 2022)

NOTE: 

- Currently Windows compatibility only works for `PinocchioPlus***`.
- **There will be runtime error in debug mode**, which might be fixed up in the future.
- Only use premake5 to generate project files.

click `generate-vs2022.bat`, then open up `PinocchioPlus.sln` in `project-files`.

- `PinocchioPlus***` are our project implementations. We keep the code base as it is. So it can not be compiled in VS2022.
- `PinocchioJingma***` are from [jingma-git/Pinocchio](https://github.com/jingma-git/Pinocchio).

All dependencies should be included.

Example:

```
./PinocchioPlusDemoUI ../../../data/FinalBaseMesh.obj -motion ../../../data/legacy/walk.txt
```

## 2 Usage

### 2.1 Skinning options

LBS:

```
./PinocchioPlusDemoUI ../../data/FinalBaseMesh.obj -motion ../../data/legacy/walk_made_by_yulong_.txt
```

DQS:

```
./PinocchioPlusDemoUI ../../data/FinalBaseMesh.obj -dqs -motion ../../data/legacy/walk_made_by_yulong_.txt
```

### 2.2 Mesh fixing

```
# In the mesh_fix&simplify directory
python fix.py
```

## 3 Useful References

- Repository
  - [jingma-git/Pinocchio](https://github.com/jingma-git/Pinocchio)
  - [pmolodo/Pinocchio](https://github.com/pmolodo/Pinocchio)
- Videos
  - [Pinocchio: Automatic Rigging and Animation of 3D Characters](https://www.youtube.com/watch?v=EklzamltEgM)
- Additional useful papers
  - [Penalty functions for automatic rigging and animation of 3d characters](https://people.csail.mit.edu/ibaran/papers/2007-SIGGRAPH-Pinocchio-Penalty.pdf) presents more implementation details.
  - [Paper explaination](https://www.jianshu.com/p/763a682c13cc).
- Datasets
  - [RigNet](https://github.com/zhan-xu/RigNet)
