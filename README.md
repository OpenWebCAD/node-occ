# node-occ

OpenCascade nodejs extension for solid modeling.

This nodejs extension provides _solid construction_ to nodejs.
It provides a simple yet powerful javascript api to construct 3D geometry models.

This project comes with a set of V8 wrappers around OpenCascade API and a sample web application.

[![Build Status](https://travis-ci.org/OpenWebCAD/node-occ.png?branch=master)](https://travis-ci.org/OpenWebCAD/node-occ)
[![Build status](https://ci.appveyor.com/api/projects/status/s5eaux89v2c0wmu4?svg=true)](https://ci.appveyor.com/project/erossignon/node-occ-6ktv4)

### quick example

```javascript
var occ = require("node-occ").occ;

// construct a box
var box = occ.makeBox([0, 0, 0], [100, 100, 50]);

// construct a cylinder
var cyl = occ.makeCylinder([50, 50, -10], [50, 50, 60], 40);

// cut the box with cylinder
box = occ.cut(box, cyl);

// save result to a STEP file
occ.writeSTEP("somefile.step", box);
```

### video

<a href="http://www.youtube.com/watch?feature=player_embedded&v=swUPSa2zyrY" target="_blank"><img src="http://img.youtube.com/vi/swUPSa2zyrY/0.jpg" 
alt="node occ" width="240" height="180" border="10" /></a>

### list of features

-   creation of basic shapes ( box, cylinder , cone , torus )
-   boolean operation ( fuse , common , cut )
-   features ( draftAngle)
-   solid properties ( faces, edges, vertices, area , volume )
-   import export ( STEP BREP )

### sample web application

[node-occ-sample](https://github.com/erossignon/node-occ-sample): sample nodejs/express REST API server to build solid , based on threejs

## installing node-occ from npm

```
$npm install node-occ
```

## building node-occ from source : prerequisites

### On Linux

#### On Ubuntu/Debian

```bash
$ git clone --recursive https://github.com/erossignon/node-occ.git
$ cd node-occ
$ make install-linux-deps
$ make install-venv-deps
$ make create-venv
$ ./venv                 # Activate the virtual environment.
(node-occ) $ ./build.sh
(node-occ) $ make test
```

### On Windows

In MSYS2 console:

```bash
$ git clone --recursive https://github.com/erossignon/node-occ.git
$ cd node-occ
$ # https://github.com/orlp/dev-on-windows/wiki/Installing-GCC--&-MSYS2
$ make install-windows-deps
$ # Better add the following in your ~/.bashrc file:
$ export PATH=/c/msys64/mingw64/bin/:/c/msys64/mingw32/bin/:$PATH

$ make install-venv-deps
$ # make sure that the patch is included: https://github.com/ekalinin/nodeenv/pull/263/files
$ # in file: /usr/lib/python3.8/site-packages/nodeenv.py
$ make create-venv
$ ./venv 
(node-occ) $ # Follow: https://github.com/nodejs/node-gyp/issues/629#issuecomment-153196245
(node-occ) $ ./build.sh
```

## dependencies:

-   threejs : https://github.com/mrdoob/three.js

## acknowledgement:

-   OpenCascade : http://www.opencascade.org
-   occmodel : https://github.com/tenko/occmodel
-   ShapeSmith : https://github.com/bjnortier/shapesmith

## MIT License

Copyright © 2012-2019 E. Rossignon

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
