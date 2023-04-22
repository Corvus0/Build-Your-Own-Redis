CXX := g++
CXXFLAGS := -Wall -Wextra -O2 -g
BLD := ./build
objects := common.o hashtable.o avl.o zset.o heap.o thread_pool.o

all: server client

server: $(objects) src/server.cpp
	$(CXX) $(CXXFLAGS) $(foreach obj,$(objects),$(BLD)/$(obj)) src/server.cpp -o $@

client: common.o src/client.cpp
	$(CXX) $(CXXFLAGS) $(BLD)/common.o src/client.cpp -o $@

$(objects): %.o: src/%.cpp src/%.h | $(BLD)
	$(CXX) -c $(CXXFLAGS) $< -o $(BLD)/$@

$(BLD):
	mkdir $@

test: clean test_avl test_offset test_cmds

test_avl: avl.o tests/test_avl.cpp
	$(CXX) $(CXXFLAGS) $(BLD)/avl.o tests/test_avl.cpp -o $@
	./test_avl

test_offset: avl.o tests/test_offset.cpp
	$(CXX) $(CXXFLAGS) $(BLD)/avl.o tests/test_offset.cpp -o $@
	./test_offset

test_cmds: server client
	python3 tests/test_cmds.py

test_heap: heap.o tests/test_heap.cpp
	$(CXX) $(CXXFLAGS) $(BLD)/heap.o tests/test_heap.cpp -o $@
	./test_heap

clean:
	rm -rf $(BLD) server client test_*
