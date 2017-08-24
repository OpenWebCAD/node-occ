/* global exports,require */
/**
 * Module dependencies.
 */
const occ = require("./bindings");

exports.version = "0.12";
exports.description = "OpenCascade Wrapper for NodeJS";

// complete object prototypes with pure Javascript
require("./mesh").init();
require("./shape").init();


// forward native module
Object.keys(occ).forEach(method => exports[method] = occ[method]);


// add utilities
exports.XDir = [1, 0, 0];
exports.YDir = [0, 1, 0];
exports.ZDir = [0, 0, 1];

exports.demo = function () {
    const solid = occ.makeBox([10, 20, 30], [40, 50, 60]);
    solid.mesh;
    return solid;
}();

function f(a, w, p) {
    return ("          " + (a + 0).toFixed(p)).substr(-w);
}

occ.Point3D.prototype.toString = function () {

    return "{" +
      " x: " + f(this.x, 8, 3) + "," +
      " y: " + f(this.y, 8, 3) + "," +
      " z: " + f(this.z, 8, 3) + "" +
      " }";
};

occ.BoundingBox.prototype.toString = function () {
    return "{ nearPt: " + this.nearPt.toString() + " , farPt: " + this.farPt.toString() + "}";
};

occ.Vertex.prototype.toString = occ.Point3D.prototype.toString;

occ.Edge.prototype.toString = function () {

    return "Edge => { \n" +
      "  numVertices: " + this.numVertices + "\n" +
      "  vertices: " +
      this.getVertices().map(function (f) {
          return f.toString();
      }).join(" ") + "}";
};

