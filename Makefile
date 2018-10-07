all:
	node-pre-gyp configure
	node-pre-gyp build
	export LD_LIBRARY_PATH=$(CURDIR)/occt-7.2.0/lib:$LD_LIBRARY_PATH; \
	mocha 
packet:
	npm install mocha
	npm install assert
	npm install should
	npm install node-pre-gyp		
	

copy: 
	COPY D:\projet\oce-build\bin\Debug\*.dll build\Release

format:
	astyle --indent=spaces=4 src/*
