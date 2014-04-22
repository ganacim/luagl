#CC=gcc
CC=cc
#CXX=/usr/bin/c++ -std=c++11 -fPIC
CXX=g++-4.8 -std=c++11 -fPIC
SO=$(CXX) -bundle -undefined dynamic_lookup
CFLAGS=-I glm -I /usr/include/lua5.2
LDFLAGS=-lglfw -lGL -llua
OBJS=glwindow.o objectviewer.o glviewer.o
DEMOS=glviewer.so

all: $(DEMOS)

glviewer.o: glviewer.cpp
	$(CXX) -c -o $@ glviewer.cpp $(CFLAGS)

box.o: box.cpp objectviewer.h glwindow.h
	$(CXX) -c -o $@ box.cpp $(CFLAGS)

glwindow.o: glwindow.cpp glwindow.h
	$(CXX) -c -o $@ glwindow.cpp $(CFLAGS)

objectviewer.o: objectviewer.cpp objectviewer.h glwindow.h
	$(CXX) -c -o $@ objectviewer.cpp $(CFLAGS)

glviewer.so: $(OBJS)
	$(SO) -o $@ $^ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(DEMOS) *.o
