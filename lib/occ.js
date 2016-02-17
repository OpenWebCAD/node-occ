/**
 * Module dependencies.
 */
var occ = require('./bindings');

exports.version="0.11";
exports.description="OpenCascade Wrapper for NodeJS";

// complete object prototypes with pure Javascript
require('./mesh').init();
require('./shape').init();



// forward native module
for (var method in occ) {    exports[method] = occ[method];}



// add utilities
exports.XDir = [1,0,0];
exports.YDir = [0,1,0];
exports.ZDir = [0,0,1];

exports.demo = function () {
   var solid = occ.makeBox([10,20,30],[40,50,60]);
   var mesh = solid.mesh;
   return solid;
}();


occ.Point3D.prototype.toString = function () {

    return "{ \n" +
        "   x: " + this.x.toString() + ",\n" +
        "   y: " + this.y.toString() + ",\n" +
        "   z: " + this.z.toString() + "\n" +
        "}";
};
occ.BoundingBox.prototype.toString = function () {

    return "{ nearPt: " + this.nearPt.toString() + " , farPt: " + this.farPt.toString() + "}";
};
