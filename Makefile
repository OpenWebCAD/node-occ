VENV_NAME := node-occ

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

install-deps:
	sudo apt-get install cmake cmake-curses-gui g++ build-essential libtbb2 

install-venv:
	sudo apt-get install python-pip
	pip install nodeenv

create-venv:
	$(eval NODE_VERSION := $(shell echo `grep "^#node@" nodeenv.txt | cut -d@ -f2` | sed 's/^$$/system/'))
	nodeenv --requirement=./nodeenv.txt --node=$(NODE_VERSION) --prompt="($(VENV_NAME))" --jobs=4 nodeenv

