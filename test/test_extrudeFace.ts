import { Triplet, occ } from "..";
import "should";

describe("demonstrate how to extrude a face", function () {
  it("extrudeFace", function () {
    // create a planar wire close line.

    const aPnt1: Triplet = [0, 0.0, 0];
    const aPnt2: Triplet = [10, 1.0, 0];
    const aPnt3: Triplet = [10, 9.0, 0];
    const aPnt4: Triplet = [0, 10.0, 0];
    const aSegment1 = occ.makeLine(aPnt1, aPnt2);
    const aSegment2 = occ.makeLine(aPnt2, aPnt3);
    const aSegment3 = occ.makeLine(aPnt3, aPnt4);
    const aSegment4 = occ.makeLine(aPnt4, aPnt1);

    const aWire = occ.makeWire(aSegment1, aSegment2, aSegment3, aSegment4);
    aWire.isClosed.should.equal(true);
    aWire.numEdges.should.equal(4);
    aWire.numVertices.should.equal(4);

    // the vector to extrude the face along.
    const aVector: Triplet = [-2, -2.0, 10];
    const aFace = occ.makeFace(aWire);

    const myBody = occ.makePrism(aFace, aVector);

    occ.writeSTEP("extrudedFace.step", [myBody]);
  });
});
