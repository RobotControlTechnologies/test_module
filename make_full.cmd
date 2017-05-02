@echo off
cmake -E make_directory build || exit /b 1
cmake -E chdir build cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_NUMBER=1 -DAPI_VERSION=101 %* || exit /b 1
cmake --build build --target install --clean-first || exit /b 1
cmake build -DCMAKE_BUILD_TYPE=DEBUG -DBUILD_NUMBER=1 -DAPI_VERSION=101 || exit /b 1
cmake --build build --target install --clean-first || exit /b 1
