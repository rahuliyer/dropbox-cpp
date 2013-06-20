CXX=g++
AR=ar
INCLUDES=-I. -I/usr/include
FLAGS=-Wall -Werror -g -std=gnu++11
DEFINES=-DHAVE_CONFIG_H
LIBRARY_INCLUDES=-L/usr/lib -L.

COMMON_LIBS=-lcurl

UTIL_OBJS=util/HttpRequestFactory.o util/HttpRequest.o util/OAuth.o
DROPBOX_OBJS=DropboxAccountInfo.o DropboxMetadata.o DropboxRevisions.o \
	DropboxApi.o
OBJS=$(UTIL_OBJS) $(DROPBOX_OBJS)

all: libdropbox.a test.o
	$(CXX) $(INCLUDES) $(FLAGS) $(LIBRARY_INCLUDES) $(DEFINES) test.o	\
		libdropbox.a $(COMMON_LIBS) -o test

libdropbox.a: $(OBJS)
	$(AR) rcs libdropbox.a $(OBJS)

%.o: %.cpp
	$(CXX) $(INCLUDES) $(FLAGS) $(DEFINES) -c $< -o $@

util/%.o: util/%.cpp
	$(CXX) $(INCLUDES) $(FLAGS) $(DEFINES) -c $< -o $@

.PHONY : clean
clean:
	rm -f *.o util/*.o test
