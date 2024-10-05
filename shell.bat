@echo off

:: uncomment the line below line to debug the vcvars
:: set VSCMD_DEBUG=1
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

:: Add dependencies to our include and lib paths (used by cl.exe)
set INCLUDE=C:\Users\Usser\Favorites\Box1\devlib\SDL2-2.30.8\include;%INCLUDE%
set LIB=C:\Users\Usser\Favorites\Box1\devlib\SDL2-2.30.8\lib\x64;%LIB%

cmd
