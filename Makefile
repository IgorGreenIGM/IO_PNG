### __PROJECT_MAKEFILE__ ###

CC = g++
# compiling in 32-bits cause zlib doesn't support 64-bits.
CFLAGS = -m32 -std=c++17 
LDFLAGS = -m32 -L"./lib" -lopengl32 -lglut32 -lz 
EXEC = bin/output.exe

all : $(EXEC)

$(EXEC): main.o CRC32.o IHDR_CHUNK.o PHYS_CHUNK.o IDAT_CHUNK.o IEND_CHUNK.o PNG.o Utilities.o
		$(CC) -o $(EXEC) $^ $(LDFLAGS)

main.o:	src/main.cpp
		$(CC) -c $< $(CFLAGS)

CRC32.o: src/PNG/CRC32.cpp
		$(CC) -c $< $(CFLAGS)

IHDR_CHUNK.o: src/PNG/Chunks/IHDR_CHUNK.cpp
		$(CC) -c $< $(CFLAGS)

PHYS_CHUNK.o: src/PNG/Chunks/PHYS_CHUNK.cpp
		$(CC) -c $< $(CFLAGS)

IDAT_CHUNK.o: src/PNG/Chunks/IDAT_CHUNK.cpp
		$(CC) -c $< $(CFLAGS)
	
IEND_CHUNK.o: src/PNG/Chunks/IEND_CHUNK.cpp
		$(CC) -c $< $(CFLAGS)

PNG.o: src/PNG/PNG.cpp
		$(CC) -c $< $(CFLAGS)

Utilities.o: src/PNG/Utilities.cpp
		$(CC) -c $< $(CFLAGS)

clean:
		rm *.o

mrproper: clean 
		rm -f $(EXEC)