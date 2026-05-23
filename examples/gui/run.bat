@echo off
setlocal

set "MODE=both"

if "%~1"=="-r" set "MODE=renderer"
if "%~1"=="-f" set "MODE=firmware"

set "PATH=%USERPROFILE%\.platformio\penv\Scripts;%PATH%"

if "%MODE%"=="firmware" goto firmware
if "%MODE%"=="renderer" goto renderer

:firmware
if exist .pio rmdir /s /q .pio

pio run -t upload
if errorlevel 1 exit /b %errorlevel%

if "%MODE%"=="firmware" goto done

:renderer
if exist renderer\build rmdir /s /q renderer\build

cmake -S renderer -B renderer\build -G Ninja ^
  -DCMAKE_C_COMPILER=gcc ^
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
  -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic ^
  -DCMAKE_PREFIX_PATH=C:/tools/msys64/mingw64
if errorlevel 1 exit /b %errorlevel%

cmake --build renderer\build
if errorlevel 1 exit /b %errorlevel%

copy /Y C:\vcpkg\installed\x64-mingw-dynamic\debug\bin\SDL2d.dll renderer\build\
copy /Y C:\tools\msys64\mingw64\bin\libserialport-0.dll renderer\build\
if errorlevel 1 exit /b %errorlevel%

.\renderer\build\tinyrtos_gui_renderer.exe

:done
endlocal
