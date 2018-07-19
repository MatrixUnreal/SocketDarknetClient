all:
	g++ -std=c++14 -o socketclient *.cpp  -lstdc++fs -lpthread `pkg-config opencv  --cflags --libs`  \
	~/lib/darknet/darknet.so -DOPENCV=1