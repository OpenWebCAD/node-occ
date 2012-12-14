var assert = require('assert');
var occ = require('./build/Release/occ');

console.log("hello");

var box1 = new occ.Solid();

assert(box1.isNull);
assert(!box1.isValid);

box1.makeBox(100,100,100);
assert(!box1.isNull);
assert(box1.isValid);

var box2 = new occ.Solid();
box1.makeBox([10,20,30],[100,100,100]);
var box3 = new occ.Solid();
box3.makeBox([10,20,30],100,100,100);

console.log("box1.x",box1.location.x);
console.log("box1.x",box1.location.y);
console.log("box1.x",box1.location.z);
console.log("cuboid",JSON.stringify(box1));
