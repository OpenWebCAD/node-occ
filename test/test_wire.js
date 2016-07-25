
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
    describe("wire with three segments", function () {

        var edge1, edge2, edge3;
        var wire;
        before(function () {
            var v1 = new occ.Vertex(0, 0, 0);
            var v2 = new occ.Vertex(10, 10, 0);
            var v3 = new occ.Vertex(20, 0, 0);
            edge1 = occ.createLine(v1, v2);
            edge2 = occ.createLine(v2, v3);
            edge3 = occ.createLine(v3, v1);
            wire = new occ.Wire(edge1, edge2, edge3);
        });

        it("should have three edges", function () {
            wire.numEdges.should.equal(3);
        });

        it("should have three vertive", function () {
            wire.numVertices.should.equal(3);
        });

        it("should  be closed", function () {
            wire.isClosed.should.equal(true);
        });
    });

});



