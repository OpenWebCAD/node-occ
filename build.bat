ECHO OFF
pushd "%~dp0"
ECHO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ %~f0 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ECHO ------------------------------------------------------------
ECHO Building module NODE-OCC
ECHO ------------------------------------------------------------

REM free disk space
df

if "%1"=="32"  set PLATFORM=x32
if "%1"=="64"  set PLATFORM=x64
if /I "%1"=="x32" set PLATFORM=x32
if /I "%1"=="x86" set PLATFORM=x32
if /I "%1"=="x64" set PLATFORM=x64


IF NOT DEFINED PLATFORM (
   echo "USAGE : BUILD 32|64"
   exit /B 1
)

IF /I "%PLATFORM%"=="x32" (
    SET npm_config_arch=ia32
) ELSE (
    SET npm_config_arch=x64
)


SET npm_config_target=
SET npm_config_disturl=
SET npm_config_runtime=


ECHO ------------------------------------------------------------
ECHO  Tool chain version
ECHO ------------------------------------------------------------
ECHO NODE
@call node --version
ECHO NPM
@call npm --version

SET GYP_MSVS_VERSION=%VSVER%

ECHO PREFIX = %PREFIX%

cd /d %~dp0"

IF DEFINED electron_version (goto buildelectron)

ECHO ---------------------------------------------------------------
ECHO ---    BUILDING FOR NODEJS
ECHO ---------------------------------------------------------------
CALL npm install --build-from-source --msvc_version=%VSVER%  --target_arch=%npm_config_arch%
if  NOT '%ERRORLEVEL%'=='0'   goto handle_msbuild_error

REM ----------------------------------------------------------
ECHO ---    TESTING FOR NODE
REM ----------------------------------------------------------
CALL npm test
if  NOT '%ERRORLEVEL%'=='0' (goto handle_test_error)

goto done

:buildelectron
ECHO ---------------------------------------------------------------
ECHO ---    BUILDING FOR ELECTRON
ECHO ---------------------------------------------------------------
cd /d %~dp0"
REM free disk space
df


IF NOT DEFINED electron_version (
   SET electron_version=1.4.14
)

SET npm_config_target=%electron_version%
SET npm_config_disturl=https://atom.io/download/atom-shell
SET npm_config_runtime=electron

ECHO Install electron and electron-mocha
ECHO "  PLATFORM            =" %PLATFORM%
ECHO "  npm_config_arch     =" %npm_config_arch%
ECHO "  npm_config_target   =" %npm_config_target%
ECHO "  npm_config_runtime  =" %npm_config_runtime%

CALL npm install electron@%electron_version% -g
CALL npm install electron-mocha
CALL npm install --build-from-source --msvc_version=%VSVER% --target_arch=%npm_config_arch%
if  NOT '%ERRORLEVEL%'=='0'   goto handle_npm_install_error

REM ----------------------------------------------------------
ECHO ---    TESTING FOR ELECTRON
REM ----------------------------------------------------------
ECHO testing dummy electron application
CALL electron test_electron
if  NOT '%ERRORLEVEL%'=='0'   goto handle_test_error
node_modules\.bin\electron-mocha test
if  NOT '%ERRORLEVEL%'=='0'   goto handle_test_error

goto done

REM REM ----------------------------------------------------------
REM REM  PACKAGE
REM REM ----------------------------------------------------------
REM SET PACKAGE=node-occ-package.zip
REM 7z a %PACKAGE% .\build\Release\*.*
REM appveyor PushArtifact %PACKAGE%

goto done
:handle_npm_install_error
:handle_test_error
:handle_cmake_error
:handle_msbuild_error
ECHO " ############################ ERROR "
goto done

:done_already_build
ECHO %PREFIX% already exists .. skipping build phase
goto done

:done
popd
pushd "%~dp0"
