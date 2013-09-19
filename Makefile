CXX=g++
AR=ar
GTEST_INCLUDES=-I/home/rni/gmock-svn/gtest/include
GTEST_LIBS=-pthread -lgmock
INCLUDES=-I. -I/usr/include $(GTEST_INCLUDES)
FLAGS=-Wall -Werror -g -std=gnu++11
DEFINES=-DHAVE_CONFIG_H
LIBRARY_INCLUDES=-L/usr/lib -L. -L/home/rni/gmock-svn/

COMMON_LIBS=-lcurl

UTIL_OBJS=util/HttpRequestFactory.o util/HttpRequest.o util/OAuth.o
DROPBOX_OBJS=DropboxAccountInfo.o DropboxMetadata.o DropboxRevisions.o \
	DropboxApi.o 
OBJS=$(UTIL_OBJS) $(DROPBOX_OBJS)

all: libdropbox.a test.o tester.o
	$(CXX) $(INCLUDES) $(FLAGS) $(LIBRARY_INCLUDES) $(DEFINES) \
    test.o libdropbox.a $(COMMON_LIBS) -o test

	$(CXX) $(INCLUDES) $(GTEST_INCLUDES) $(FLAGS) $(LIBRARY_INCLUDES) $(DEFINES) \
    tester.o libdropbox.a $(COMMON_LIBS) $(GTEST_LIBS) -o tester

libdropbox.a: $(OBJS)
	$(AR) rcs libdropbox.a $(OBJS)

%.o: %.cpp
	$(CXX) $(INCLUDES) $(FLAGS) $(DEFINES) -c $< -o $@

util/%.o: util/%.cpp
	$(CXX) $(INCLUDES) $(FLAGS) $(DEFINES) -c $< -o $@

.PHONY : clean
clean:
	rm -f *.o util/*.o test
