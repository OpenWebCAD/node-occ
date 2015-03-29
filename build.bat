git submodule update --init --recursive
 mkdir build_oce
cd build_oce
cmake -D OCE_INSTALL_PREFIX:STRING='C:/OCE-17' -D OCE_DRAW:BOOLEAN=FALSE -D OCE_OCAF:BOOLEAN=FALSE -D  OCE_VISUALISATION:BOOLEAN=FALSE -D OCE_DISABLE_X11:BOOLEAN=TRUE -D OCE_USE_PCH:BOOLEAN=TRUE  ../oce  
msbuild oce.sln
msbuild ocd.sln /p:Configuration=Release
set VisualStudioVersion=11.0
msbuild INSTALL.vcxproj /p:Configuration=Release

cd ..
SET CL=/Ic:\OCE-0.17\include\oce
SET LINK=/LIBPATH:c:\OCE-0.17\Win32\lib
PATH c:\OCE-0.17\Win32\bin;%PATH%
npm install
