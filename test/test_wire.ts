import "should";

import { IWire, occ } from "..";

describe("testing Wire ", function () {
  describe("empty wire", function () {
    let wire: IWire;
    before(function () {
      wire = occ.makeWire();
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
    let wire1: IWire, wire2: IWire, wire3: IWire;
    let wire: IWire;
    before(function () {
      let v1 = occ.makeVertex(0, 0, 0);
      let v2 = occ.makeVertex(10, 10, 0);
      let v3 = occ.makeVertex(20, 0, 0);

      wire1 = occ.makeLine(v1, v2);
      wire2 = occ.makeLine(v2, v3);
      wire3 = occ.makeLine(v3, v1);

      wire = occ.makeWire(wire1, wire2, wire3);
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
