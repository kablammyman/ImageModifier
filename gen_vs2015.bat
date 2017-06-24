if not exist "build_vs2015" mkdir build_vs2015

cd build_vs2015

cmake -G "Visual Studio 14 2015 Win64" ..

cd ..