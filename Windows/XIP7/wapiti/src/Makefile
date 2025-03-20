#### ATTENTION, pour compiler la version 32 bits, il faut que c:\strawberry corresponde a c:\strawberry32
#### Pour la version 64 bits: il faut que c:\strawberry corresponde a c:\strawberry64...
#### Ne pas oublier de correctement initialiser la version de strawberry

# source files.
SRC =  	bcd.c decoder.c gradient.c lbfgs.c model.c options.c pattern.c progress.c quark.c reader.c rprop.c sgdl1.c thread.c tools.c vmath.c wapiti.c
SRCPP = tagger.cpp extractFeature.cpp
SRCE = wap.cpp

OBJ = $(addprefix obj32/,$(SRC:.c=.o))
OBJCPP = $(addprefix obj32/,$(SRCPP:.cpp=.o))

OBJ64 = $(addprefix obj64/,$(SRC:.c=.o))
OBJCPP64 = $(addprefix obj64/,$(SRCPP:.cpp=.o))

OBJE = $(SRCE:.cpp=.o)

OUT = ../wapiti.dll

# include directories
INCLUDES = -I. -IH:/ANTKIF/regex/dev/include


# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -m32 -std=c99 -shared -DWINDLL
CCPFLAGS = -m32 -shared -DWINDLL

CCFLAGS64 = -m64 -std=c99 -shared -DWAPITILIB -DWINDLL
CCPFLAGS64 = -m64 -shared -DWAPITILIB -DWINDLL

# compiler
CCPP = g++
CC = gcc

# library paths
LIBSDLL = -LC:/strawberry/c/i686-w64-mingw32/lib -L.. -mwindows -lm -lboost_regex
LIBS = -LC:/strawberry/c/i686-w64-mingw32/lib -L.. -lm

LIBSDLL64 = -LC:/strawberry/c/x86_64-w64-mingw32/lib -L.. -mwindows -lm -lboost_regex
LIBS64 = -LC:/strawberry/c/x86_64-w64-mingw32/lib -L.. -lm

# compile flags
LDFLAGS = 

.SUFFIXES: .cpp

default: $(OUT)

obj32/%.o: %.c
	$(CC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

obj32/%.o: %.cpp
	$(CCPP) $(INCLUDES) $(CCPFLAGS) -c $< -o $@

obj64/%.o: %.c
	$(CC) $(INCLUDES) $(CCFLAGS64) -c $< -o $@

obj64/%.o: %.cpp
	$(CCPP) $(INCLUDES) $(CCPFLAGS64) -c $< -o $@

#$(OUT): $(OBJ) $(OBJCPP)
#	ar rcs $(OUT) $(OBJ) $(OBJCPP)

liba: $(OBJ64) $(OBJCPP64)
	$(CCPP) -shared -o $(OUT) $(OBJ64) $(OBJCPP64) $(LIBSDLL64) -Wl,--out-implib,../libwapiti.a

dll32: $(OBJ) $(OBJCPP)
	$(CCPP) -shared -o ../wapiti32.dll $(OBJ) $(OBJCPP) $(LIBSDLL) -Wl,--out-implib,../wapiti32.lib
	
dll64: $(OBJ64) $(OBJCPP64)
	$(CCPP) -shared -o ../wapiti64.dll $(OBJ64) $(OBJCPP64) $(LIBSDLL64) -Wl,--out-implib,../wapiti64.lib

clean:
	rm -f $(OBJ64) $(OBJCPP64) $(OBJE) ../wap.exe $(OUT) ../wapiti.lib ../libwapiti.a Makefile.bak 

exe: $(OBJE)
	$(CCPP64) -o ../wap.exe $(OBJE) $(LIBS64) -lwapiti
	
