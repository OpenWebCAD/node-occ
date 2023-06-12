"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const __1 = require("..");
require("should");
describe("demonstrate how to extrude a face", function () {
    it("extrudeFace", function () {
        // create a planar wire close line.
        const aPnt1 = [0, 0.0, 0];
        const aPnt2 = [10, 1.0, 0];
        const aPnt3 = [10, 9.0, 0];
        const aPnt4 = [0, 10.0, 0];
        const aSegment1 = __1.occ.makeLine(aPnt1, aPnt2);
        const aSegment2 = __1.occ.makeLine(aPnt2, aPnt3);
        const aSegment3 = __1.occ.makeLine(aPnt3, aPnt4);
        const aSegment4 = __1.occ.makeLine(aPnt4, aPnt1);
        const aWire = __1.occ.makeWire(aSegment1, aSegment2, aSegment3, aSegment4);
        aWire.isClosed.should.equal(true);
        aWire.numEdges.should.equal(4);
        aWire.numVertices.should.equal(4);
        // the vector to extrude the face along.
        const aVector = [-2, -2.0, 10];
        const aFace = __1.occ.makeFace(aWire);
        const myBody = __1.occ.makePrism(aFace, aVector);
        __1.occ.writeSTEP("extrudedFace.step", [myBody]);
    });
});
