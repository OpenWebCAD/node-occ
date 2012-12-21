node-occ
========

node extension for OpenCascade.

This node extension provides solid construction to node.



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


''/sample'' : sample nodejs/express REST API server to build solid

