/**
 * Module dependencies.
 */
var occ = require('./bindings')

exports.version="0.11";
exports.description="OpenCascade Wrapper for NodeJS";

// complete object prototypes with pure Javascript
require('./mesh').init();
require('./shape').init();



// forward native module
for (var method in occ) {    exports[method] = occ[method];}

exports.makeBox1 = function()
{
   var solid = new occ.Solid();
   return solid.makeBox.apply(solid,arguments);
}

exports.makeCylinder1= function()
{
    var solid = new occ.Solid();
    return solid.makeCylinder.apply(solid,arguments);
}

exports.makeSphere1 = function()
{
    var solid = new occ.Solid();
    return solid.makeSphere.apply(solid,arguments);
}

exports.cut1 = function()
{
    var solid = new occ.Solid();
    return solid.cut.apply(solid,arguments);
}

exports.fuse1 = function()
{
    var solid = new occ.Solid();
    return solid.cut.apply(solid,arguments);
}

exports.common1 = function()
{
    var solid = new occ.Solid();
    return solid.common.apply(solid,arguments);
}

// add utilities
exports.XDir = [1,0,0];
exports.YDir = [0,1,0];
exports.ZDir = [0,0,1];

exports.demo = function () {
   var solid = occ.makeBox([10,20,30],[40,50,60]);
   var mesh = solid.mesh;
   return solid;
}();