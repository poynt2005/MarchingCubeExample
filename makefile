cc = gcc
cxx = g++
cflags = -I./freeglut/include -I./glfw/include
ldflags = -L./freeglut/lib -L./glfw/lib
libs = -lfreeglut -lopengl32 -lGlu32 -lglfw3dll

dll:
	$(cxx) -fPIC -shared -std=c++17 -c MarchingCube.cc -o MarchingCube.o
	$(cxx) -fPIC -shared $(cflags) -c Drawler.cc -o Drawler.o
	$(cxx) -fPIC -shared -std=c++17 -DBUILDMCAPI -c MarchingCubeAPI.cc -o MarchingCubeAPI.o
	$(cxx) -fPIC -shared -DBUILDDRAPI -c DrawlerAPI.cc -o DrawlerAPI.o

	$(cxx) -shared MarchingCube.o MarchingCubeAPI.o -Wl,--out-implib,MarchingCubeAPI.lib -o MarchingCubeAPI.dll
	$(cxx) -shared $(ldflags) DrawlerAPI.o Drawler.o -Wl,--out-implib,DrawlerAPI.lib -o DrawlerAPI.dll $(libs)

dr:
	$(cxx) -I./libuv/include -c testDrawler.cc -o testDrawler.o
	$(cxx) -L./ -L./libuv/lib testDrawler.o -o testDrawler.exe -lMarchingCubeAPI -lDrawlerAPI -luv


test:
	$(cc) -c test.c -o test.o
	$(cc) -L./ test.o -o test.exe -lMarchingCubeAPI