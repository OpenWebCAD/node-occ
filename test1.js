var util   = require('util');
var assert = require('assert');
var occ = require('./');



function testBufferArray()
{

   var b = new Buffered
}
console.log("hello");

var box1 = new occ.Solid();

assert(box1.isNull);
assert(!box1.isValid);

box1.makeBox([10,20,30],[30,40,50]);

//xx for (var i in box1.mesh) {
//xx     console.log( " i=",i,"  => ", box1.mesh[i]);
//xx }
console.log("---------------!!!");

console.log( " i=",i,"  => ", box1.mesh.vertices);

assert(!box1.isNull);
assert(box1.isValid);

var box2 = new occ.Solid();
box1.makeBox([10,20,30],[100,100,100]);
var box3 = new occ.Solid();
box3.makeBox([10,20,30],100,100,100);

//xx console.log("box1.x",box1.location.x);
//xx console.log("box1.x",box1.location.y);
//xx console.log("box1.x",box1.location.z);
//xx console.log("cuboid",JSON.stringify(box1));


var solid = new occ.Solid();
solid.makeBox([10,20,30],[20,30,40]);


for ( var i in solid) {
	console.log(" i = ", i, " => " , solid[i]);
}

var expected = ["isNull","area","volume","numFaces","numSolids","isValid","rotate","fuse"];

for ( var j in occ.Solid.prototype) {
	console.log(" j = ", j, " => " ,  occ.Solid.prototype[j]);
}	


console.log("------------------");



console.log("JSON =",solid.mesh.toJSON());


occ.writeSTEP("toto.STEP",solid);

console.log("Done");
