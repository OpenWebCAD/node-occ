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

        //xx topFace.area.should.equal(100.0);

        topFace.hasMesh.should.equal(false);
        // now mesh the solid
        var m = solid.mesh;
        topFace.hasMesh.should.equal(true);

        // topFace.mesh.toJSON();
    });
});

