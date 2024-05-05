# PinocchioPlus

This repository is conducting a research on the paper "Automatic Rigging and animation of 3D characters".

## Installation

### Ubuntu 18.04

[jingma-git/Pinocchio](https://github.com/jingma-git/Pinocchio) dependencies:
- `sudo apt-get install libfltk1.3-dev`
- `sudo apt-get install libxrender-dev libxcursor-dev libxft-dev libfontconfig-dev libxinerama-dev`

#### cmake

```
# PinocchioJingma directory
cmake ..
make
```

#### premake

```
# root directory
./generate-gmake2.sh 
cd project-files
make
```

Then you can check out the `bin` directory.


Example:

```
./PinocchioJingmaDemoUI ../../../obj_files/2982_remesh.obj -motion ../../../PinocchioJingma/data/walk.txt
```

## Useful References

- Repository
  - [jingma-git/Pinocchio](https://github.com/jingma-git/Pinocchio)
  - [pmolodo/Pinocchio](https://github.com/pmolodo/Pinocchio)
- Videos
  - [Pinocchio: Automatic Rigging and Animation of 3D Characters](https://www.youtube.com/watch?v=EklzamltEgM)
- Additional useful papers
  - [Penalty functions for automatic rigging and animation of 3d characters](https://people.csail.mit.edu/ibaran/papers/2007-SIGGRAPH-Pinocchio-Penalty.pdf) presents more implementation details.
  - (https://www.jianshu.com/p/763a682c13cc) Paper explaination.
- Datasets
  - [RigNet](https://github.com/zhan-xu/RigNet)
