@echo off
setlocal


call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

set SRC=src
set INC=include
set OBJ=obj
set SFML=C:\SFML-2.6.1\SFML-2.6.1
set CFLAGS=/std:c++20 /EHsc /I%INC% /I%SFML%\include

for %%f in (%SRC%\*.cpp) do (
    echo Compiling %%f ...
    cl /c %CFLAGS% %%f /Fo%OBJ%\
)

link %OBJ%\*.obj ^
/LIBPATH:%SFML%\lib ^
sfml-graphics.lib ^
sfml-window.lib ^
sfml-system.lib ^
/OUT:symulacja.exe

echo.
echo Build complete!
pause