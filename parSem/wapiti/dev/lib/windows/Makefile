#### ATTENTION, pour compiler il faut installer a la fois Strawberry perl 64bits et MinGW
### Pour mingw:   www.mingw.org
### Pour Strawberry: strawberryperl.com
### Ensuite, il fait lance msys.bat dans MinGW/msys/1.0
### La lib libregex a ete recompilee sous MinGW a partir de msys.bat
### En effectuant les operations suivantes:
### ./bjam.exe toolset=gcc --with-regex
### ./b2.exe link=static variant=release cxxflags="-m64" cflags="-m64" toolset=gcc regex
### Il faut aussi les libs suivantes pour l'execution (dans strawberry\perl\bin): libgcc_s_sjlj-1.dll,libstdc++-6.dll,libwinpthread-1.dll

# source files.
SRC =  	bcd.c decoder.c gradient.c lbfgs.c model.c options.c pattern.c progress.c quark.c reader.c rprop.c sgdl1.c thread.c tools.c vmath.c wapiti.c
SRCPP = tagger.cpp extractFeature.cpp
SRCE = wap.cpp

OBJ64 = $(addprefix obj64/,$(SRC:.c=.o))
OBJCPP64 = $(addprefix obj64/,$(SRCPP:.cpp=.o))

OBJE = $(SRCE:.cpp=.o)

OUT = ../wapiti.dll

# include directories
INCLUDES = -I. -IH:/ANTKIF/regex/dev/include


# C++ compiler flags (-g -O2 -Wall)
CCFLAGS64 = -m64 -std=c99 -shared -DWAPITILIB -DWINDLL
CCPFLAGS64 = -m64 -shared -DWAPITILIB -DWINDLL

# compiler
CCPP = g++
CC = gcc

# library paths
LIBSDLL64 = -LC:/strawberry/c/x86_64-w64-mingw32/lib -L.. -mwindows -lm -lboost_regex
LIBS64 = -LC:/strawberry/c/x86_64-w64-mingw32/lib -L.. -lm

# compile flags
LDFLAGS = 

.SUFFIXES: .cpp

default: $(OUT)

obj64/%.o: %.c
	$(CC) $(INCLUDES) $(CCFLAGS64) -c $< -o $@

obj64/%.o: %.cpp
	$(CCPP) $(INCLUDES) $(CCPFLAGS64) -c $< -o $@

#$(OUT): $(OBJ) $(OBJCPP)
#	ar rcs $(OUT) $(OBJ) $(OBJCPP)
	
dll64: $(OBJ64) $(OBJCPP64)
	$(CCPP) -shared -o ../wapiti64.dll $(OBJ64) $(OBJCPP64) $(LIBSDLL64) -Wl,--out-implib,../wapiti64.lib

clean:
	rm -f $(OBJ64) $(OBJCPP64) $(OBJE) ../wap.exe $(OUT) ../wapiti.lib ../libwapiti.a Makefile.bak 

exe: $(OBJE)
	$(CCPP64) -o ../wap.exe $(OBJE) $(LIBS64) -lwapiti

