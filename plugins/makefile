GPP = g++ -m32 -std=c++11 -Ilib -Isrc -fno-stack-protector -nodefaultlibs
GCC = gcc -m32 -Ilib -Isrc -fno-stack-protector -nodefaultlibs
LINK = $(GCC)
PP_OUTFILE = "./LFN.so"

COMPILE_FLAGS = -c -O3 -fPIC -w -DLINUX -pthread -fno-operator-names

LFN = -D LFN $(COMPILE_FLAGS)

all: LFN

clean:
	-rm -f *~ *.o *.so
  
static: GPP = g++ -m32 -std=c++11 -Ilib -Isrc -fno-stack-protector -static-libgcc -static-libstdc++
static: GCC = gcc -m32 -Ilib -Isrc -fno-stack-protector -static-libgcc -static-libstdc++
static: LINK = $(GPP)
static: all

LFN: clean
	$(GPP) $(LFN) ./lib/sdk/*.cpp
	$(GCC) $(LFN) ./lib/subhook/*.c
	$(GPP) $(LFN) ./src/*.cpp
	$(LINK) -fshort-wchar -pthread -shared -o $(PP_OUTFILE) *.o
