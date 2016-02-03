all: libfreetype_wrapper.a

CPPFLAGS=-I/usr/include/freetype2 

libfreetype_wrapper.a: freetype_wrapper.o string_helpers.o
	$(AR) rcs $@ $^
	
%.o: %.cpp
	$(CXX) -std=c++11 -I/usr/include/freetype2 $^ -c -o $@

clean:
	rm *.o *.a
