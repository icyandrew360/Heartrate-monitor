CXX = gcc
CXXFLAGS = -Wall -g

all: HRM FitnessTracker

HRM: HRM.o
	$(CXX) $(CXXFLAGS) -o HRM HRM.o

HRM.o: HRM.c
	$(CXX) $(CXXFLAGS) -c HRM.c

FitnessTracker: FitnessTracker.o
	$(CXX) $(CXXFLAGS) -o FitnessTracker FitnessTracker.o

FitnessTracker.o: FitnessTracker.c
	$(CXX) $(CXXFLAGS) -c FitnessTracker.c

