CXX = g++
CXXFLAGS = -std=c++17

FILE = IDA_inocente
PDBCREATOR_1 = bfs_1
PDBCREATOR_2 = bfs_2
TEST = automatic_test

all: database IDA generate_database

IDA: 
	$(CXX) $(CXXFLAGS) -o exe/$(FILE) $(FILE).cpp

database: 
	$(CXX) $(CXXFLAGS) -o exe/$(PDBCREATOR_1) $(PDBCREATOR_1).cpp
	$(CXX) $(CXXFLAGS) -o exe/$(PDBCREATOR_2) $(PDBCREATOR_2).cpp

generate_database:
	./exe/$(PDBCREATOR_1)
	./exe/$(PDBCREATOR_2)

tests:
	./exe/$(TEST)
