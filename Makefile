VENV_NAME := node-occ
ifeq ($(MSYSTEM),MSYS)
$(info MSYS Environment)
PKG_INSTALL := pacman -S
else
$(info Linux Environment)
PKG_INSTALL := sudo apt-get install 
endif

__check_venv:
	$(if $(NODE_VIRTUAL_ENV),,$($(warning ************  WARNING: NOT INSIDE A VIRTUAL ENV  ************)))

all:
	node-pre-gyp configure
	node-pre-gyp build
	mocha

.PHONY: test

test: __check_venv
	mocha

clean:
	node-pre-gyp clean

packet:
	npm install mocha
	npm install assert
	npm install should
	npm install node-pre-gyp

copy:
	COPY D:\projet\oce-build\bin\Debug\*.dll build\Release

format:
	astyle --indent=spaces=4 src/*

install-linux-deps:
	$(PKG_INSTALL) cmake cmake-curses-gui g++ build-essential libtbb2 

install-win-deps:
	pacman -Syuu
	$(PKG_INSTALL) --needed base-devel mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain git subversion mercurial mingw-w64-i686-cmake mingw-w64-x86_64-cmake unzip

install-venv-deps:
	$(PKG_INSTALL) python-pip
	pip install nodeenv

create-venv:
	$(eval NODE_VERSION := $(shell cat .node-version.txt))
	nodeenv --requirement=./nodeenv.txt --node=$(NODE_VERSION) --prompt="($(VENV_NAME))" --jobs=4 nodeenv

