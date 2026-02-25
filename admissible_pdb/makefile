CXX = g++
CXXFLAGS = -std=c++17

FILE = IDA_inocente
PDBCREATOR = bfs
TEST = automatic_test

all: database IDA generate_database

IDA: 
	$(CXX) $(CXXFLAGS) -o exe/$(FILE) $(FILE).cpp

database: 
	$(CXX) $(CXXFLAGS) -o exe/$(PDBCREATOR) $(PDBCREATOR).cpp

generate_database:
	./exe/$(PDBCREATOR)

tests:
	./exe/$(TEST)
