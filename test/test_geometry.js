var assert = require("assert");

var occ = require("../lib/occ");
var shapeFactory = require('../lib/shapeFactory');


var getTemporaryFilePath = require('gettemporaryfilepath');


describe("testing geometry builder",function(){
  it("should create a bottle", function() {

    var bottle_brep = getTemporaryFilePath({ prefix: "bottle", suffix: ".brep"});

    var bottle = shapeFactory.makeBottle();
    occ.writeBREP(bottle_brep,bottle);
  });
});

