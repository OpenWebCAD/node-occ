lscpu ;

#set NPROC=`nproc`
NPROC=$(grep "^core id" /proc/cpuinfo | wc -l)

echo "Numbrer of processors =" ${NPROC}
if [ "$TRAVIS" = "true" ]
then
dpkg -i packages/OCCT-7.0_trusty-amd64.deb
else
cd occt ;
wget "http://git.dev.opencascade.org/gitweb/?p=occt.git;a=snapshot;h=b00770133187b83761e651df50051b2fa3433858;sf=tgz";
mv "index.html?p=occt.git;a=snapshot;h=b00770133187b83761e651df50051b2fa3433858;sf=tgz" occt.tgz;
gunzip occt.tgz;
tar xf occt.tar;
rm occt.tar;
mv occt-b007701 occt-7.0.0;
cd occt-7.0.0;
grep -v vtkRenderingFreeTypeOpenGL src/TKIVtk/EXTERNLIB > /tmp/EXTERNLIB;
cp /tmp/EXTERNLIB src/TKIVtk/EXTERNLIB ;
grep -v vtkRenderingFreeTypeOpenGL src/TKIVtkDraw/EXTERNLIB > /tmp/EXTERNLIB;
cp /tmp/EXTERNLIB src/TKIVtkDraw/EXTERNLIB;
mkdir build;
cd build;
cmake .. -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/ -DUSE_VTK:BOOL=OFF ;

echo 1 `date +%R:%S`;
echo "CPU COUNT " ${NPROC}
make -j ${NPROC} install 
cd ..
echo "done"
fi
