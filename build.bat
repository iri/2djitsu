@echo off
 
set buildDir=%~dp0build
 
if not exist %buildDir% mkdir %buildDir%
 
pushd %buildDir%

:: compiler input
set objDir=%buildDir%\obj\
set outputExe=%buildDir%\2dj
set libs=SDL2.lib SDL2main.lib SDL2_image.lib shell32.lib
set source=%buildDir%\..\code\2dj.cpp
set INCLUDE=%buildDir%\..\code;%INCLUDE%


:: compiler flags:
:: /Zi enable debugging information
:: /FC use full path in diagnostics
:: /Fo<path> the path to write object files
:: /Fe<path> the path to write the executable file
set compileFlags=/Zi /FC /Fo%objDir% /Fe%outputExe%  /EHsc

:: linker flags:
:: /SUBSYSTEM specifies exe env - defines entry point symbol
set linkFlags=/link /SUBSYSTEM:CONSOLE

if not exist %objDir% mkdir %objDir%

cl %compileFlags% %source% %libs% %linkFlags%

copy %outputExe%.exe ..

popd

