.PHONY: build

all: build test

build:
	./node_modules/.bin/node-pre-gyp configure
	./node_modules/.bin/node-pre-gyp build

test:	
	mocha test/*.ts

.PHONY: test
test:
	mocha test/*.ts

clean:
	./node_modules/.bin/node-pre-gyp clean
copy: 
	COPY D:\projet\oce-build\bin\Debug\*.dll build\Release


## https://leimao.github.io/blog/Clang-Format-Quick-Tutorial/
format:
	clang-format -i src/*.h src/*.cc
	npx -y prettier -w lib/*.ts test/*.ts
	
