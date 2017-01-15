
const occ = require("../lib/occ");
const should = require("should");

describe("demonstrate how to extrude a face",function() {

    it("extrudeFace",function() {

        // create a planar wire close line.

        let  aPnt1 = [   0 ,  0.0 , 0];
        let  aPnt2 = [  10 ,  1.0 , 0];
        let  aPnt3 = [  10 ,  9.0 , 0];
        let  aPnt4 = [   0 , 10.0 , 0];
        let aSegment1 = new occ.makeLine(aPnt1, aPnt2);
        let aSegment2 = new occ.makeLine(aPnt2, aPnt3);
        let aSegment3 = new occ.makeLine(aPnt3, aPnt4);
        let aSegment4 = new occ.makeLine(aPnt4, aPnt1);

        let aWire  = new occ.Wire(aSegment1,aSegment2,aSegment3,aSegment4);
        aWire.isClosed.should.equal(true);
        aWire.numEdges.should.equal(4);
        aWire.numVertices.should.equal(4);

        // the vector to extrude the face along.
        let aVector = [  -2 , -2.0 , 10];
        let aFace = new occ.Face(aWire);

        let myBody = occ.makePrism(aFace,aVector);

        occ.writeSTEP("extrudedFace.step",[myBody]);

    });
});
