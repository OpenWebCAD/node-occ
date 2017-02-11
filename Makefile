all:
	node-pre-gyp configure
	node-pre-gyp build
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
