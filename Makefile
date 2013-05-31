CXX=g++
AR=ar
INCLUDES=-I. -I/usr/include
FLAGS=-Wall -Werror -g -std=gnu++11
DEFINES=-DHAVE_CONFIG_H
LIBRARY_INCLUDES=-L/usr/lib -L.

COMMON_LIBS=-lcurl

all: util/HttpRequestFactory.o util/HttpRequest.o
	$(CXX) $(INCLUDES) $(FLAGS) $(LIBRARY_INCLUDES) $(DEFINES) test.cpp util/HttpRequestFactory.o util/HttpRequest.o $(COMMON_LIBS) -o test

%.o: %.cpp
	$(CXX) $(INCLUDES) $(FLAGS) $(DEFINES) -c $< -o $@

util/%.o: util/%.cpp
	$(CXX) $(INCLUDES) $(FLAGS) $(DEFINES) -c $< -o $@

.PHONY : clean
clean:
	rm -f *.o util/*.o test
