var occ = require("./lib/occ");

// create the top wire
//             ---|
//           .    .
//         /      |
// 2+-----+       .
//  |     3       |
// 1+-------------.
//  |<------------w--...>
//

var w = 65;
var r = 20;
var h = 12;

console.log("t2");
var p0 = occ.makeVertex(0, 0, 0);
console.log("t3");
var p1 = p0.translate(-w / 2.0, 0, 0);
console.log("t4");
var p2 = p1.translate(0.0, h, 0);
console.log("t5");
var p3 = p0.translate(-r, h, 0);
console.log("t6");
var p4 = p0.translate(0, h + r, 0);
console.log("t7");

var trsf = occ.makePlaneMirror([0, 0, 0], [1, 0, 0]);
console.log("t8");

var q1 = p1.transformed(trsf);
console.log("t2");
var q2 = p2.transformed(trsf);
console.log("t2");
var q3 = p3.transformed(trsf);
console.log("t2");
var q3 = p3.transformed(trsf);
console.log("t2");

var e1 = occ.createLine(q1, p1);
console.log("1t2");
var e2 = occ.createLine(p1, p2);
console.log("2t2");
var e3 = occ.createLine(p2, p3);
console.log("3t2");
var e4 = occ.createArc3P(p3, p4, q3);
console.log("4t2");
var e5 = occ.createLine(q3, q2);
console.log("5t2");
var e6 = occ.createLine(q2, q1);
console.log("6t2");

var wire1 = occ.makeWire(e1, e2, e3, e4, e5, e6);
console.log("t2");

var face1 = occ.makeFace(wire1);
console.log("t2");

var height = 12;
console.log("t2");
var body1 = occ.makePrism(face1, [0, 0, height]);
console.log("t2");

console.log(body1);
console.log("t2");
