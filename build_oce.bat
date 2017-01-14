ECHO OFF
pushd "%~dp0"
ECHO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ %~f0 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ECHO ----------------------------------------------------------
ECHO -- Building OCE
ECHO ----------------------------------------------------------

if "%1"=="32"  set PLATFORM=x32
if "%1"=="64"  set PLATFORM=x64
if /I "%1"=="x32" set PLATFORM=x32
if /I "%1"=="x86" set PLATFORM=x32
if /I "%1"=="x64" set PLATFORM=x64

ECHO PLATFORM %PLATFORM%  - %1

IF NOT DEFINED PLATFORM (
   echo "USAGE : BUILD 32|64"
   exit /B 1
)
ECHO PLATFORM %PLATFORM%

IF /I "%PLATFORM%" == "x64" (
    SET VCPLATFORM=x64
    ECHO Compiling for x64
    goto VS2013_X64
) ELSE IF /I "%PLATFORM%" == "x32" (
    SET VCPLATFORM=Win32
    ECHO Compiling for x86
    goto VS2013_X86
) else (
    ECHO INVALID PLATFORM SPECIFIED
    goto handle_test_error
)

REM goto VS2015_X64
REM goto VS2015_X64


:VS2012_X86
ECHO ---------------------------------------------------------------------------
ECHO  Compiling with Visual Studio 2012 - X86
ECHO ---------------------------------------------------------------------------
SET VSVER=2012
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
CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64
goto do_cmake


:VS2013_X64
REM ----------------------------------------------------------------------------
REM   Compiling with Visual Studio 2013 - x64
REM ----------------------------------------------------------------------------
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
CALL "%~dp0"/SETENV.BAT  32
set GENERATOR=Visual Studio 12 2013
set VisualStudioVersion=12.0
CALL "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
goto do_cmake

:do_cmake

ECHO PREFIX  : %PREFIX%
IF EXIST %PREFIX% GOTO done_already_build


ECHO ----------------------------------------------------------
ECHO  PREPARE : git submodule
ECHO ----------------------------------------------------------
CALL git submodule update --init --recursive


ECHO NODE-PRE-GYP
@call node-pre-gyp --version
@call cl
@call msbuild


ECHO "----------------------------------------------------------------------"
ECHO "CMAKE"
ECHO "----------------------------------------------------------------------"

SET BUILD_OCE=build_oce_%VSVER%_%ARCH%
ECHO PREFIX      = "%PREFIX%"
ECHO CL          = "%CL%"
ECHO LINK        = "%LINK%"
ECHO PATH        = "%PATH%"
ECHO ARCH        = "%ARCH%"
ECHO VCPLATFORM  = "%VCPLATFORM%"
ECHO BUILD_OCE   = "%BUILD_OCE%"
ECHO GENERATOR   = "%GENERATOR%"

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
CALL msbuild /m oce.sln /p:Configuration=Release /p:Platform=%VCPLATFORM% /verbosity:%VERBOSITY% /consoleloggerparameters:Summary;ShowTimestamp
ECHO ERROR LEVEL = %ERRORLEVEL%
if NOT '%ERRORLEVEL%'=='0' goto handle_msbuild_error

ECHO ---------- Installing to "%PREFIX%"
CALL msbuild /m INSTALL.vcxproj /p:Configuration=Release  /p:Platform=%VCPLATFORM%  /verbosity:%VERBOSITY% /consoleloggerparameters:Summary;ShowTimestamp
if NOT ERRORLEVEL 0  goto handle_msbuild_error

ECHO ---------- Installing to "%PREFIX%"
REM CALL msbuild /m INSTALL.vcxproj /p:Configuration=Debug  /p:Platform=%VCPLATFORM%  /verbosity:%VERBOSITY% /consoleloggerparameters:Summary;ShowTimestamp
REM if NOT ERRORLEVEL 0  goto handle_msbuild_error

goto done

:handle_test_error
:handle_cmake_error
:handle_msbuild_error
ECHO " ############################ ERROR "


:done_already_build

ECHO ----------------------------------------------------------
ECHO %PREFIX% already exists .. skipping build phase
ECHO ----------------------------------------------------------
goto done

:done
ECHO .....
ECHO Done!
ECHO .....
popd
pushd "%~dp0"
