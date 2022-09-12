cd %cd%

g++^
 "src/main.cpp"^
 "src/PNG/CRC32.cpp"^
 "src/PNG/Chunks/IHDR_CHUNK.cpp"^
 "src/PNG/Chunks/PHYS_CHUNK.cpp"^
 "src/PNG/Chunks/IDAT_CHUNK.cpp"^
 "src/PNG/Chunks/IEND_CHUNK.cpp"^
 "src/PNG/PNG.cpp"^
 "src/PNG/Utilities.cpp"^
 -c -L"./lib" -m32 -lopengl32 -lglut32 -lz

@echo off
echo.
echo.
IF %errorlevel% == 0 echo [Application compiled succeffully]  &&^
echo [Starting moving files...] &&^
echo. &&^
move *.o "./bin/link"

IF %errorlevel% == 1 echo Application compilation failed

echo.
pause