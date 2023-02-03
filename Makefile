CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -g
objects = utils.o

all: server client

server: $(objects) server.cpp
	$(CXX) $(CXXFLAGS) $(objects) server.cpp -o server

client: $(objects) client.cpp
	$(CXX) $(CXXFLAGS) $(objects) client.cpp -o client

$(objects): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) utils.cpp -o utils.o

clean:
	rm -f client server utils.o
