require('assert');
var occ = require('./build/Release/binding');


var cuboid = occ.Cuboid({ position: [10,20,30], size: [100,100,100]});

console.log("cuboid",JSON.stringify(cuboid));
