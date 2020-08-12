#!/bin/bash
#set -e
##########################################################################################
#
#
##########################################################################################
export OCCT_VERSION=7.2.0
export OCCT_PACKAGE=occt-${OCCT_VERSION}
INCLUDE_DIR="include/opencascade"
if [ `uname` == "Darwin" ];then
  export OCCT_TARFILE=${OCCT_PACKAGE}-osx.tgz
elif [ `uname` == "Linux" ];then
  export OCCT_TARFILE=${OCCT_PACKAGE}-linux.tgz
elif [ "$MSYSTEM" == "MSYS" ]; then
  INCLUDE_DIR="inc"
  export OCCT_TARFILE=${OCCT_PACKAGE}-win64.zip
fi
export OCCT_TARFILE_URL="https://github.com/OpenWebCAD/occt_builder/releases/download/${OCCT_VERSION}/${OCCT_TARFILE}"

echo "  OCCT_TARFILE     = " ${OCCT_TARFILE}
echo "  OCCT_TARFILE_URL = " ${OCCT_TARFILE_URL}

echo "--------------------------  OCCT TAR FILE ${OCCT_TARFILE}"
if [ ! -f "${OCCT_TARFILE}" ]; then
  echo "Getting ${OCCT_TARFILE}"
  wget --continue -O "${OCCT_TARFILE}.downloading" ${OCCT_TARFILE_URL} --progress=bar:force 2>&1 | tail -f -n +6
  mv "${OCCT_TARFILE}.downloading" ${OCCT_TARFILE}
fi

if [ ! -d "${OCCT_PACKAGE}" ]; then 
  echo "extracting package ${OCCT_TARFILE}"
  extension="${OCCT_TARFILE##*.}"
  if [ "$extension" == "zip" ]; then
    unzip ${OCCT_TARFILE}
  else
    tar -xf ${OCCT_TARFILE}
  fi
fi
 
export LD_LIBRARY_PATH=`pwd`/${OCCT_PACKAGE}/lib:$LD_LIBRARY_PATH

export VERSION_FILE=`pwd`/${OCCT_PACKAGE}/${INCLUDE_DIR}/Standard_Version.hxx
grep -i "#define OCC_VERSION_COMPLETE"  ${VERSION_FILE} ;
lscpu 2> /dev/null;
cmake --version ;

#set NPROC=`nproc`
# on linux:
#    NPROC=$(grep "^core id" /proc/cpuinfo | sort -u | wc -l)
# on MacOS
#    NPROC=$(sysctl -n hw.ncpu)
# on both
NPROC=$(node -p "os.cpus().length")

echo "Number of processors =" ${NPROC}


PATH=$PATH:`pwd`/node_modules/.bin
uname -a
echo "NODE PATH    = `which node`"
echo "NODE VERSION = `node --version`"
echo "NODE         = `node -p 'process.platform + "@" + process.arch'`"
echo "NPM VERSION  = `npm --version`"


# figure out if we should publish
PUBLISH_BINARY=false

# if we are building a tag then publish
echo $TRAVIS_BRANCH
export CURRENT BRANCH=`git describe --tags --always HEAD`
echo "CURRENT BRANCH  = $CURRENT_BRANCH" 
if [[ "$TRAVIS_BRANCH" == "$CURRENT_BRANCH" ]]; then PUBLISH_BINARY=true; fi;


echo "Publishing native platform Binary Package? ->" $PUBLISH_BINARY

# Cleanup the output of npm
npm config set progress false
npm config set spin false

if [[ ! -z $TRAVIS_ELECTRON_VERSION ]]; then
  if [[ $TRAVIS_OS_NAME == "linux" ]]; then
    export DISPLAY=:99.0
    sh -e /etc/init.d/xvfb start
  fi
  export npm_config_target=$TRAVIS_ELECTRON_VERSION
  export npm_config_arch=$ARCH
  export npm_config_disturl=https://atom.io/download/atom-shell
  export npm_config_runtime=electron
  npm install --arch=$ARCH electron@$TRAVIS_ELECTRON_VERSION
  npm install electron-mocha
  echo "installed Electron $TRAVIS_ELECTRON_VERSION"
fi

#  git submodule update --depth 50 --init --recursive
[[ ${1:-} = "--only-prepare" ]] || npm install --build-from-source 
