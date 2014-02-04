grep -i VERSION /usr/include/oce/Standard_Version.hxx ;
cmake --version ;
cd oce ;
git checkout tags/OCE-0.13 ;
cd .. ;
mkdir deps ;
mkdir build_oce;
cd build_oce ;
cmake -D OCE_INSTALL_PREFIX:STRING='/usr/local' -D OCE_DRAW:BOOLEAN=FALSE -D OCE_OCAF:BOOLEAN=FALSE -D  OCE_VISUALISATION:BOOLEAN=FALSE -D OCE_DISABLE_X11:BOOLEAN=TRUE -D OCE_USE_PCH:BOOLEAN=TRUE  ../oce > /dev/null;
echo 1;
make -j 72 TKernel > /dev/null;
echo 2;
make -j 72 TKMath > /dev/null;
echo 3;
make -j 72 TKG2d > /dev/null;
make -j 72 TKGeomBase   > /dev/null;
make -j 72 TKG3d > /dev/null;
make -j 72 TKPrim > /dev/null;
make -j 72 TKBO   > /dev/null;
make -j 72 TKBRep   > /dev/null;
make -j 72 TKXmlL  > /dev/null;
make -j 72 TKShHealing  > /dev/null;
make -j 72 TKPLCAF  > /dev/null;
make -j 72 TKPShape  > /dev/null;
make -j 72 TKSTEPBase  > /dev/null;
make -j 72 TKSTEP209  > /dev/null;
make -j 72 TKSTEPAttr  > /dev/null;
make -j 72 TKIGES  > /dev/null;
make -j 72 TKSTEP  > /dev/null;
make -j 72 ;
sudo make install > /dev/null  ;
sudo ldconfig ;

cd ..
ls -R deps
