cd %cd%

g++^
 "bin/link/main.o"^
 "bin/link/CRC32.o"^
 "bin/link/IHDR_CHUNK.o" ^
 "bin/link/PHYS_CHUNK.o" ^
 "bin/link/IDAT_CHUNK.o" ^
 "bin/link/IEND_CHUNK.o" ^
 "bin/link/PNG.o" ^
 "bin/link/Utilities.o" ^
 -o "./bin/output.exe"^
 -L"./lib" -m32 -lopengl32 -lglut32 -lz

@echo off
echo.
echo.
IF %errorlevel% == 0 echo Application compiled succeffully
IF %errorlevel% == 1 echo Application compilation failed
echo.
pause