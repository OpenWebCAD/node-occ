# node-occ

OpenCascade nodejs extension for solid modeling.

This nodejs extension provides *solid construction* to nodejs. 
It provides a simple yet powerful javascript api to construct 3D geometry models.

This project comes with a set of V8 wrappers around OpenCascade API and a sample web application.


### quick example


```javascript
var occ = require('occ')

// construct a box
var box = occ.makeBox([0,0,0],[100,100,50])

// construct a cylinder
var cyl = occ.makeCylinder([50,50,-10],[50,50,60],40)

// cut the box with cylinder
box = occ.cut(box,cyl)

// save result to a STEP file
occ.writeSTEP("somefile.step",box)

```


### video
<a href="http://www.youtube.com/watch?feature=player_embedded&v=swUPSa2zyrY" target="_blank"><img src="http://img.youtube.com/vi/swUPSa2zyrY/0.jpg" 
alt="node occ" width="240" height="180" border="10" /></a>


### list of features

- creation of basic shapes ( box, cylinder , cone , torus )
- boolean operation ( fuse , common , cut )
- features  ( draftAngle)
- solid properties ( faces, edges, vertices, area , volume )
- import export ( STEP BREP )



### sample web application

''/sample'' : sample nodejs/express REST API server to build solid , based on threejs



## installing node-occ from npm
 
### on Windows
  - you need Microsoft Visual Studio 2012
  - please install OpenCascade oce-0.13  from https://github.com/tpaviot/oce/downloads to you c:\OCE-0.13 folder
  
### on Linux




## building node-occ :  prerequisites

### on ubuntu

installing node js
```bash

# installing nodejs and gyp utility to build extensions
sudo apt-get install nodejs npm
sudo npm install node-gyp -g
sudo npm install mocha -g

#ddd installing prebuild opencascade
#ddd sudo apt-get install oce-draw

------------------------------------
# building opencascade from oce
#installing cmake
sudo apt-get install cmake cmake-curses-gui g++ build-essential
# fetching source code
git clone https://github.com/tpaviot/oce.git
# switching to official release 0.13
cd oce
git checkout tags/OCE-0.13
cd ..
# creating a build area for oce
make build_oce
cd build_oce
cmake -D  OCE_VISUALISATION:BOOLEAN=FALSE -D OCE_DISABLE_X11:BOOLEAN=TRUE -D OCE_USE_PCH:BOOLEAN=TRUE  ../oce

# building oce
make -j 4

cd ..
git clone https://github.com/erossignon/node-occ.git
cd node-occ
npm install
node-gyp configure
node-gyp build
mocha

cd sample
npm install
node app.js
```


### on windows
  - please install OCE-0.13  from https://github.com/tpaviot/oce/downloads to you c:\OCE-0.13 folder


### on Mac


## dependencies:

 - threejs    : https://github.com/mrdoob/three.js
 - codemirror : https://github.com/marijnh/CodeMirror

## acknowledgement:
    
 - OpenCascade : http://www.opencascade.org
 - occmodel    : https://github.com/tenko/occmodel
 - ShapeSmith  : https://github.com/bjnortier/shapesmith



## MIT License

Copyright © 2012-2013 E. Rossignon

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
