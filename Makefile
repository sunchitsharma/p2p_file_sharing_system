
CXX = g++
CXXFLAGS = -Wall -std=c++11

client_20172037:
	$(CXX) $(CXXFLAGS) client.cpp -o $@ -lpthread

server_20172037:
	$(CXX) $(CXXFLAGS) crs.cpp -o $@ -lpthread


