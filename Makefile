
CXX := g++
LD  := g++
CXXFLAGS := -std=c++11 -O0 -Wall -pedantic
LDFLAGS  := -lSDL2 -lSDL2main
SRCFILES := $(wildcard src/*.cc)
OBJFILES := $(patsubst src/%.cc, obj/%.cc.o, $(SRCFILES))
OUTFILE  := ray

.PHONY: clean rebuild all

all: $(OUTFILE)

rebuild: $(CLEAN) all

clean:
	$(RM) $(OUTFILE) obj/*.o

$(OUTFILE): $(OBJFILES) Makefile
	$(LD) -o $@ $(OBJFILES) $(LDFLAGS)

obj/%.cc.o: src/%.cc Makefile
	$(CXX) -o $@ $< $(CXXFLAGS) -c
