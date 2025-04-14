This is a simple game I made to practice creating superbuilds with cmake and getting comfortable with the new SFML 3.0 repository. 

Should be very easy to get running. 

On windows cmake will fetch all needed libraries just follow steps 1-6 below.

On linux you will have to install the following libraries with sudo apt
```
sudo apt update
sudo apt install \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libfreetype-dev \
    libflac-dev \
    libvorbis-dev \
    libgl1-mesa-dev \
    libegl1-mesa-dev \
    libfreetype-dev

```


Basic Prerequisites:
```
CMake 3.0 , C++17 compiler (GCC, Clang, MSVC) , Git , build tool (make on Linux or Visual Studio on Windows)
```

1. clone repository
`git clone https://github.com/PricetonB/SpaceWar.git`

2. create build directory and cd into it
`mkdir build && cd build`

3. configure with cmake
`cmake ..`

4. build with cmake
`cmake --build .`

5. cd into directory with executable
`cd bin\debug (cd bin on linux)`

6. execute the program
`.\main.exe\ (./main on linux)`
