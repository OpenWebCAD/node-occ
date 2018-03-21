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
SET OCCT_VER=7.2.0
SET OCCT_FOLDER="%CWD%occt-%OCCT_VER%\"
ECHO %OCCT_FOLDER%
IF EXIST "%OCCT_FOLDER%" (
   echo OCCT-%OCCT_VER% prebuild stuff already installed
) else (
  IF NOT EXIST occt-%OCCT_VER% (
      curl -OL https://github.com/OpenWebCAD/occt_builder/releases/download/%OCCT_VER%/occt-%OCCT_VER%-win64.zip
  )
  7z x occt-%OCCT_VER%-win64.zip -y > a.txt
  echo DONE
)

::
:: Note CL and LINK require unix style like path
::
SET OCE=occt-%OCCT_VER%
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

