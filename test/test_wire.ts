import "should";

import { IEdge, IWire, Triplet, occ } from "..";

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

  describe("wire made of many small segments", () => {

    const r = 100;

    const segments: IWire[] = [];

    for (let i = 0; i <= 360; i += 10) {
      const a1 = i * Math.atan(1) / 45;
      const a2 = (i + 1) * Math.atan(1) / 45;
      const c1 = Math.cos(a1);
      const c2 = Math.cos(a2);
      const s1 = Math.sin(a1);
      const s2 = Math.sin(a2);

      const p1: Triplet = [r * c1, r * s1, 0];
      const p2: Triplet = [r * c2, r * s2, 0];

      console.log(p1, p2);
      const s = occ.makeLine(p1, p2);
      segments.push(s);
    }

    const wire = occ.makeWire(segments);
  });


});
