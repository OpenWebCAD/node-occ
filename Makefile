all:
	node-gyp configure
	node-gyp build
	mocha 
packet:
	npm install mocha
	npm install assert
	npm install should
	npm install should		
	

copy: 
	COPY D:\projet\oce-build\bin\Debug\*.dll build\Release
