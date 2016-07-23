ECHO OFF
pushd "%~dp0"

ECHO ----------------------------------------------------------
ECHO  PREPARE : git submodule
ECHO ----------------------------------------------------------
CALL git submodule update --init --recursive


ECHO ------------------------------------------------------------
ECHO  Tool chain version
ECHO ------------------------------------------------------------
CL
node-gyp -v
msbuild

ECHO PREFIX  : %PREFIX%
IF EXIST %PREFIX% GOTO done_already_build

REM goto VS2013_X86
goto VS2015_X64

:VS2012_X86
ECHO ---------------------------------------------------------------------------
ECHO  Compiling with Visual Studio 2012 - X86
ECHO ---------------------------------------------------------------------------
SET VSVER=2012
SET PLATFORM=Win32
CALL "%~dp0"/SETENV.BAT  32
set GENERATOR=Visual Studio 11 2012
set VisualStudioVersion=11.0
CALL "%VS110COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
goto do_cmake

:VS2015_X86
ECHO ---------------------------------------------------------------------------
ECHO  Compiling with Visual Studio 2015 - X86
ECHO ---------------------------------------------------------------------------
SET VSVER=2015
SET PLATFORM=Win32
CALL "%~dp0"/SETENV.BAT  32
set GENERATOR=Visual Studio 14 2015
set VisualStudioVersion=14.0
CALL "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" x86
goto do_cmake

:VS2015_X64
ECHO ---------------------------------------------------------------------------
ECHO  Compiling with Visual Studio 2015 - X64
ECHO ---------------------------------------------------------------------------
SET VSVER=2015
CALL "%~dp0"/SETENV.BAT  64
set GENERATOR=Visual Studio 14 2015 Win64
set VisualStudioVersion=14.0
CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" x64
goto do_cmake


:VS2013_X64
REM ----------------------------------------------------------------------------
REM   Compiling with Visual Studio 2013 - x64
REM ----------------------------------------------------------------------------
SET PLATFORM=x64
SET VSVER=2013
CALL "%~dp0"/SETENV.BAT  64
set GENERATOR=Visual Studio 12 2013 Win64
set VisualStudioVersion=12.0
CALL "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64
goto do_cmake

:VS2013_X86
ECHO ---------------------------------------------------------------------------
ECHO  Compiling with Visual Studio 2013 - X86
ECHO ---------------------------------------------------------------------------
SET VSVER=2013
SET PLATFORM=Win32
CALL "%~dp0"/SETENV.BAT  32
set GENERATOR=Visual Studio 12 2013
set VisualStudioVersion=12.0
CALL "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
goto do_cmake

:do_cmake

SET BUILD_OCE=build_oce_%VSVER%_%ARCH%
ECHO PREFIX = "%PREFIX%"
ECHO CL     = "%CL%"
ECHO LINK   = "%LINK%"
ECHO PATH   = "%PATH%"
ECHO ARCH   = "%ARCH%"
ECHO BUILD_OCE   = "%BUILD_OCE%"

mkdir %BUILD_OCE%
cd  %BUILD_OCE%
CALL cmake -DOCE_INSTALL_PREFIX:STRING="%PREFIX%" ^
-DCMAKE_SUPPRESS_REGENERATION:BOOL=ON  ^
-DOCE_MULTITHREADED_BUILD:BOOL=ON ^
-DBUILD_SHARED_LIBS:BOOL=OFF ^
-DBUILD_TESTING:BOOLEAN=OFF ^
-DOCE_DRAW:BOOLEAN=OFF ^
-DOCE_TESTING:BOOLEAN=OFF ^
-DOCE_OCAF:BOOLEAN=OFF ^
-DOCE_VISUALISATION:BOOLEAN=OFF ^
-DOCE_DISABLE_X11:BOOLEAN=ON ^
-DOCE_DISABLE_TKSERVICE_FONT:BOOLEAN=ON ^
-DOCE_USE_PCH:BOOLEAN=ON  ^
-G "%GENERATOR%" ../oce

if NOT ERRORLEVEL 0 goto handle_cmake_error

SET VERBOSITY=quiet
REM SET VERBOSITY=minimal

REM msbuild /m oce.sln
CALL msbuild /m oce.sln /p:Configuration=Release /p:Platform=%PLATFORM% /verbosity:%VERBOSITY% /consoleloggerparameters:Summary;ShowTimestamp
ECHO ERROR LEVEL = %ERRORLEVEL%
if NOT '%ERRORLEVEL%'=='0' goto handle_msbuild_error

CALL msbuild /m INSTALL.vcxproj /p:Configuration=Release  /p:Platform=%PLATFORM%  /verbosity:%VERBOSITY% /consoleloggerparameters:Summary;ShowTimestamp
if NOT ERRORLEVEL 0  goto handle_msbuild_error


REM CALL msbuild /m INSTALL.vcxproj /p:Configuration=Debug  /p:Platform=%PLATFORM%  /verbosity:%VERBOSITY% /consoleloggerparameters:Summary;ShowTimestamp
REM if NOT ERRORLEVEL 0  goto handle_msbuild_error

SET GYP_MSVS_VERSION=%VSVER%

ECHO PREFIX = %PREFIX%
ECHO PREFIX = %GENERATOR%


cd /d %~dp0"

REM ----------------------------------------------------------
REM  BUILD
REM ----------------------------------------------------------

CALL npm install
if  NOT '%ERRORLEVEL%'=='0'   goto handle_msbuild_error

REM ----------------------------------------------------------
REM  TEST
REM ----------------------------------------------------------
CALL npm test
if  NOT '%ERRORLEVEL%'=='0'   goto handle_test_error

REM ----------------------------------------------------------
REM  PACKAGE
REM ----------------------------------------------------------
SET SRC=%PREFIX%/Win32/bin
SET SRC=%SRC:/=\%
XCOPY %SRC%\*.dll .\build\Release
SET PACKAGE=node-occ-package.zip
7z a %PACKAGE% .\build\Release\*.*
appveyor PushArtifact %PACKAGE%

goto done

:handle_test_error
:handle_cmake_error
:handle_msbuild_error
ECHO " ############################ ERROR "


:done_already_build
ECHO %PREFIX% already exists .. skipping build phase
goto done

:done
popd
pushd "%~dp0"
