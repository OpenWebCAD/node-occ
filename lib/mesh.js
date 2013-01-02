
var occ = require('./bindings');


function toArray(meshBuffer)
{
   var res = [];
   for (var i =0;i< meshBuffer.length;i++) {
      res.push(meshBuffer[i]);
   }
   return res;
}
exports.init= function() {

	var assert = require('assert');
	console.log('adding occ.Mesh.prototype.toJSON function');
	occ.Mesh.prototype.toJSON = function() {

    // see https://github.com/mrdoob/three.js/wiki/JSON-Model-format-3.1
	var json = {
			metadata: { "formatVersion" : 3 },
			materials: [ {
				DbgColor : 15658734, // => 0xeeeeee
				DbgIndex : 0,
				DbgName : "dummy",
				colorDiffuse : [ 1, 0, 0 ]
			} ],
			vertices: [],
			normals: [],
			faces: []
	};
   
   json.vertices = toArray(this.vertices);
   json.normals  = toArray(this.normals);


   json.uvs = [];

   json.faces = [];

   var f ;
   var j =0;
   for (var i=0; i < this.numTriangles; i++) {

        json.faces.push(0+ 2*2*2*2*2 ); // vertex + normal index

        var i1 = this.triangles[i*3+0];
        var i2 = this.triangles[i*3+1];
        var i3 = this.triangles[i*3+2];

        json.faces.push(i1);
        json.faces.push(i2);
        json.faces.push(i3);
        json.faces.push(i1);
        json.faces.push(i2);
        json.faces.push(i3);


   }

    // return json;
    return JSON.stringify(json);
};

console.log(" Hello");

};
