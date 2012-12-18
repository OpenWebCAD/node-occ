/**
 * Module dependencies.
 */

var occ = require('./bindings')


// complete definition of Mesh
var Solid = occ.Solid
var Mesh = occ.Mesh

m = require('./mesh');
console.log("m =",m);
m.init();


exports.version="0.11";
exports.description="OpenCascade Wrapper for NodeJS";



exports.Solid = occ.Solid;
exports.Mesh = occ.Mesh;
exports.Vertex = occ.Vertex;
exports.Edge = occ.Edge;
exports.Wire = occ.Wire;

exports.writeSTEP = occ.writeSTEP;

exports.demo = function () {
   var solid = new occ.Solid();
   solid.makeBox([10,20,30],[40,50,60]);
   var mesh = solid.mesh;
   
   return solid;
}();