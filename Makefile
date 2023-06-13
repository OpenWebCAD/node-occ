all:
	./node_modules/.bin/node-pre-gyp configure
	./node_modules/.bin/node-pre-gyp build
	mocha test/*.ts

.PHONY: test
test:
	mocha test/*.ts

clean:
	./node_modules/.bin/node-pre-gyp clean		  
	²
copy: 
	COPY D:\projet\oce-build\bin\Debug\*.dll build\Release

format:
	astyle --indent=spaces=4 src/*
