
var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");


describe("testing Wire ",function(){

    describe("empty wire", function() {
        var wire;
        before(function() {
            wire = new occ.Wire();
        });
        it("should have no edges", function() {
            wire.numEdges.should.equal(0);
        });
        it("should have no vertex", function() {
            wire.numVertices.should.equal(0);
        });
        it("should not be closed", function() {
            wire.isClosed.should.equal(false);
        });
    });
});



