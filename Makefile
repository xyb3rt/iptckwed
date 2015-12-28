VERSION  = git-20151228
PREFIX   = /usr/local

CXX      = g++
CXXFLAGS = -Wall -pedantic -O2
CPPFLAGS = -I$(PREFIX)/include
LDFLAGS  = -L$(PREFIX)/lib
LIBS     = -lexiv2

SRC = iptckwed.cpp
OBJ = $(SRC:.cpp=.o)

all: iptckwed

$(OBJ): Makefile

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -DVERSION=\"$(VERSION)\" -c -o $@ $<

iptckwed:	$(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	rm -f $(OBJ) iptckwed

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp iptckwed $(DESTDIR)$(PREFIX)/bin/
	chmod 755 $(DESTDIR)$(PREFIX)/bin/iptckwed

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/iptckwed
