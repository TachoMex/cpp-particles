# sudo apt install freeglut3-dev libfmt-dev binutils-dev

g++ main.cpp -lGL -lGLU -lglut -o paint -std=c++2a -lfmt -lbfd -g
