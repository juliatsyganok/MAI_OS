
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude

all: libimpl1.so libimpl2.so program1 program2

libimpl1.so: src/lib1.cpp include/contract.h
	$(CXX) $(CXXFLAGS) -fPIC -shared src/lib1.cpp -o $@

libimpl2.so: src/lib2.cpp include/contract.h
	$(CXX) $(CXXFLAGS) -fPIC -shared src/lib2.cpp -o $@

program1: src/program1.cpp libimpl1.so
	$(CXX) $(CXXFLAGS) src/program1.cpp -L. -limpl1 -Wl,-rpath,'$$ORIGIN' -o $@

program2: src/program2.cpp include/contract.h
	$(CXX) $(CXXFLAGS) src/program2.cpp -ldl -o $@


clean:
	rm -f libimpl1.so libimpl2.so program1 program2

.PHONY: all test clean
