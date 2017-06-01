@ECHO OFF
:: SETENV.BAT

set ARCH=unset

if '%1' == '32' set ARCH=Win32
if '%1' == '64' set ARCH=Win64
if '%ARCH%' == 'unset' (
    echo "USAGE : SETENV 32|64"
    exit /B 1
)


SET CWD=%~dp0
SET OCCT_FOLDER="%CWD%occt-7.1.0\"
ECHO %OCCT_FOLDER%
IF EXIST "%OCCT_FOLDER%" (
   echo OCCT-7.1.0 prebuild stuff already installed
) else (
  IF NOT EXIST occt-7.0.1 (
     curl -OL https://github.com/OpenWebCAD/occt_builder/releases/download/v7.1.0l/occt-7.1.0-win64.zip
  )
  7z x occt-7.1.0-win64.zip -y > a.txt
  echo DONE
)

::
:: Note CL and LINK require unix style like path
::
SET OCE=occt-7.1.0
SET PREFIX=%OCCT_FOLDER%
SET CL=/I%PREFIX%/inc
SET LINK=/LIBPATH:%PREFIX%/%ARCH%/vc14/lib
SET CMAKE_BIN=C:\Program Files (x86)\CMake\bin
SET GIT_BIN=C:\Program Files (x86)\Git\bin
::
:: set the find command to point to the windows version (avoiding inadvertant usage of gitbash find command)
::
SET FIND=%WINDIR%\SYSTEM32\FIND.EXE
::
:: Let check if we have already inserted element in Path
::
echo ";%PATH%;" | %FIND% /C /I ";c:\%OCE%\%ARCH%\bin;" > tmp.txt
set /p F=<tmp.txt
del tmp.txt

if "%F%" == "0"  PATH c:\%OCE%\%ARCH%\bin;%PATH%;%CMAKE_BIN%;%GIT_BIN%

ECHO CL   %CL%
ECHO LINK %LINK%
ECHO PATH %PATH%
ECHO ARCH %ARCH%


EXIT /B 0
