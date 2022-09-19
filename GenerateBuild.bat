mkdir build
cd build
cmake ..

@echo off
SET /p build="Build projects? [y/n]: "
IF %build%==y cmake --build .
@echo on

PAUSE