# hanz
Like the xbox kinect, but much, much shittier

## Why?
I had a week before I started work and betweem playing minecraft in my underwear and drinking wine alone in my room watching Game of Thrones I wanted to keep my hands coding on something. So I decided to make this abomination.

## Getting Started
If you for some reason want to use this garbage. Follow the next steps

### Prerequisites
Currently, this has been tested only on Linux 5.09-arch1-1-ARCH
- CMake >= 3.9
- Ninja (if you want)
- C++ Compiler (clang was used here)
- STDC++17
- OpenCV

Supplied versions are what this system was tested on, and it may work on lower versions, but is not guaranteed.

#### Arch Linux
```
pacman -S cmake ninja hdf5 vtk opencv
```
#### Installing
This project can be cloned anywhere on your system.

CMake supports out of source builds, so we can make a build directory (referred herein as `BUILD_DIR`) you may also see a warning if you do an in source build because it's gross to do that.
```
cd <PROJECT_ROOT>
mkdir <BUILD_DIR>
```
After the place to build our system has been created we can now use CMake to generate our build structure.
```
cd <BUILD_DIR>
cmake -GNinja ..
```
Once all the build files have been prepared, we can compile.
```
ninja
```

Following this, we can find out executables for our model types and compile them

```
cd <BIN_DIR>
./hanz
```

#### Uninstalling

To uninstall, simply execute `$ rm -r <PROJECT_ROOT>` and this will remove the files. Or simply pour 5 glasses of water on your machine.
