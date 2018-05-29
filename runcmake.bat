@echo off
@rem Running cmake on Windows example for VS2013
cls

@SETLOCAL
@SET CMAKE_EXEC="c:/prog/cmake-gui/bin/cmake.exe"
@IF EXIST %CMAKE_EXEC% GOTO START
@GOTO ERROR

:START
mkdir build
cd build

%CMAKE_EXEC% ^
-G"Visual Studio 12 Win64" ../work

cd ..
@GOTO EOF

@:ERROR
@echo ERROR: CMake not found.

:EOF
pause