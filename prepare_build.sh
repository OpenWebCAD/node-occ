grep -i VERSION /usr/include/oce/Standard_Version.hxx ;
lscpu ;
cmake --version ;

set NPROC=`nproc`
echo "Numbrer of processors =" ${NPROC}

cd oce ;
git checkout tags/OCE-0.13 ;
cd .. ;
mkdir deps ;
mkdir build_oce;
cd build_oce ;
cmake -D OCE_INSTALL_PREFIX:STRING='/usr/local' -D OCE_DRAW:BOOLEAN=FALSE -D OCE_OCAF:BOOLEAN=FALSE -D  OCE_VISUALISATION:BOOLEAN=FALSE -D OCE_DISABLE_X11:BOOLEAN=TRUE -D OCE_USE_PCH:BOOLEAN=TRUE  ../oce > /dev/null;


echo 1 `date +%R:%S`;
make -s -j ${NPROC}  TKernel > /dev/null;
echo 2 `date +%R:%S`;
make -s -j ${NPROC} TKMath > /dev/null;
echo 3 `date +%R:%S`;
make -s -j ${NPROC} TKG2d > /dev/null;
echo 4 `date +%R:%S`;
make -s -j ${NPROC} TKGeomBase   > /dev/null;
echo 5 `date +%R:%S`;
make -s -j ${NPROC} TKG3d > /dev/null;
echo 6 `date +%R:%S`;
make -s -j ${NPROC} TKPrim > /dev/null;
echo 7 `date +%R:%S`;
make -s -j ${NPROC} TKBO   > /dev/null;
echo 8 `date +%R:%S`;
make -s -j ${NPROC} TKBRep   > /dev/null;
echo 9 `date +%R:%S`;
make -s -j ${NPROC} TKXmlL  > /dev/null;
echo 10 `date +%R:%S`;
make -s -j ${NPROC} TKShHealing  > /dev/null;
echo 11 `date +%R:%S`;
make -s -j ${NPROC} TKPLCAF  > /dev/null;
echo 12 `date +%R:%S`;
make -s -j ${NPROC} TKPShape  > /dev/null;
echo 13 `date +%R:%S`;
make -s -j ${NPROC} TKSTEPBase  > /dev/null;
echo 14 `date +%R:%S`;
make -s -j ${NPROC} TKSTEP209  > /dev/null;
echo 15 `date +%R:%S`;
make -s -j ${NPROC} TKSTEPAttr  > /dev/null;
echo 16 `date +%R:%S`;
make -s -j ${NPROC} TKIGES  > /dev/null;
echo 17 `date +%R:%S`;
make -s -j ${NPROC} TKSTEP  > /dev/null;
echo 18 `date +%R:%S`;
make -s -j ${NPROC} ;
echo 19 `date +%R:%S`;

sudo make install > /dev/null  ;
sudo ldconfig ;
cd ..


