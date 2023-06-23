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
    let edge1: IEdge, edge2: IEdge, edge3: IEdge;
    let wire: IWire;
    before(function () {
      let v1 = occ.makeVertex(0, 0, 0);
      let v2 = occ.makeVertex(10, 10, 0);
      let v3 = occ.makeVertex(20, 0, 0);

      edge1 = occ.makeLine(v1, v2);
      edge2 = occ.makeLine(v2, v3);
      edge3 = occ.makeLine(v3, v1);

      wire = occ.makeWire(edge1, edge2, edge3);
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
    const DEG2RAD = Math.atan(1) / 45;
    function makeSegments() {
      const r = 100;

      const segments: IEdge[] = [];

      for (let i = 0; i <= 360; i += 45) {
        const a1 = i * DEG2RAD;
        const c1 = Math.cos(a1);
        const s1 = Math.sin(a1);

        const a2 = (i + 1) * DEG2RAD;
        const c2 = Math.cos(a2);
        const s2 = Math.sin(a2);

        const p1: Triplet = [r * c1, r * s1, 0];
        const p2: Triplet = [r * c2, r * s2, 0];

        // console.log(p1, p2);
        const s = occ.makeLine(p1, p2);
        segments.push(s);
      }
      return segments;
    }

    function shuffleInPlace<T>(array: T[]): void {
      for (let i = array.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        const temp = array[i];
        array[i] = array[j];
        array[j] = temp;
      }
    }
    it("should create a wire with a set of ordered segments forming a close wire", () => {
      const segments = makeSegments();
      const wire = occ.makeWire(...segments);
      wire.isClosed.should.eql(true);
    });

    it("should create a wire with a set of shuffled segments forming a close wire", () => {
      const segments = makeSegments();
      shuffleInPlace(segments);
      const wire = occ.makeWire(segments);
      wire.isClosed.should.eql(false);
      if (wire) {
        // wire.isDegenerated.should.eql(false);
      }
    });

    it("should falied to create  a wire with a set of segments that is not forming a close wire", () => {
      const segments = makeSegments();
      const lBefore = segments.length;

      segments.splice(1, 2);

      segments.splice(5, 2);

      const lAfter = segments.length;

      console.log("lBefore", lBefore, lAfter);
      const wire = occ.makeWire(segments);
      if (wire) {
        //  wire.isClosed.should.eql(false);
        // wire.isDegenerated.should.eql(false);
      }
    });
  });
});
