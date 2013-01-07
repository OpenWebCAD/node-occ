var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");

// see https://npmjs.org/package/should

describe("testing face mesh ",function() {

    it("should not have a mesh unless the parent solid has been meshed", function(){

        var solid = occ.makeBox([0,0,0],[10,10,10]);
        solid.faces.should.have.property("top");
        var topFace = solid.faces.top;
        should.exist(topFace);
        console.log(topFace.area);

        topFace.hasMesh.should.equal(false);
        // now mesh the solid
        solid.mesh;
        topFace.hasMesh.should.equal(true);
        console.log(topFace.mesh.toJSON())
    });
});

