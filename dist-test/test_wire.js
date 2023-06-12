"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
require("should");
const __1 = require("..");
describe("testing Wire ", function () {
    describe("empty wire", function () {
        let wire;
        before(function () {
            wire = __1.occ.makeWire();
        });
        it("should have no edges", function () {
            wire.numEdges.should.equal(0);
        });
        it("should have no vertex", function () {
            wire.numVertices.should.equal(0);
        });
        it("should not be closed", function () {
            wire.isClosed.should.equal(false);
        });
    });
    describe("wire with three segments", function () {
        let wire1, wire2, wire3;
        let wire;
        before(function () {
            let v1 = __1.occ.makeVertex(0, 0, 0);
            let v2 = __1.occ.makeVertex(10, 10, 0);
            let v3 = __1.occ.makeVertex(20, 0, 0);
            wire1 = __1.occ.makeLine(v1, v2);
            wire2 = __1.occ.makeLine(v2, v3);
            wire3 = __1.occ.makeLine(v3, v1);
            wire = __1.occ.makeWire(wire1, wire2, wire3);
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
