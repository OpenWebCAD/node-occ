CALL git submodule update --init --recursive
mkdir build_oce
cd build_oce
SET PREFIX=C:/OCE-17

set GENERATOR=Visual Studio 11 2012
set VisualStudioVersion=11.0
CALL cmake -D OCE_INSTALL_PREFIX:STRING="%PREFIX%" -D OCE_DRAW:BOOLEAN=FALSE -D OCE_OCAF:BOOLEAN=FALSE -D  OCE_VISUALISATION:BOOLEAN=FALSE -D OCE_DISABLE_X11:BOOLEAN=TRUE -D OCE_DISABLE_TKSERVICE_FONT:BOOLEAN=TRUE -D OCE_USE_PCH:BOOLEAN=TRUE  -G "%GENERATOR%" ../oce  
REM msbuild /m oce.sln
CALL msbuild /m oce.sln /p:Configuration=Release

CALL msbuild INSTALL.vcxproj /p:Configuration=Release

cd ..
SET CL=/I%PREFIX%/include/oce
SET LINK=/LIBPATH:%PREFIX%/Win32/lib
PATH %PREFIX%/Win32/bin;%PATH%
CALL npm install
