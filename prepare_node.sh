#!/bin/bash
##########################################################################################
#
#
##########################################################################################


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
if [ "$TRAVIS_BRANCH" == "$CURRENT_BRANCH" ]; then PUBLISH_BINARY=true; fi;


echo "Publishing native platform Binary Package? ->" $PUBLISH_BINARY

# Cleanup the output of npm
npm config set progress false
npm config set spin false

if [ ! -z $TRAVIS_ELECTRON_VERSION ]; then
  if [ $TRAVIS_OS_NAME == "linux" ]; then
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
npm install --build-from-source
