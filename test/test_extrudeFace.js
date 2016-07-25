
var occ = require("../lib/occ");
var should = require("should");

describe("demonstrate how to extrude a face",function() {

    it("extrudeFace",function() {

        // create a planar wire close line.

        var  aPnt1 = [   0 ,  0.0 , 0];
        var  aPnt2 = [  10 ,  1.0 , 0];
        var  aPnt3 = [  10 ,  9.0 , 0];
        var  aPnt4 = [   0 , 10.0 , 0];
        var aSegment1 = new occ.createLine(aPnt1, aPnt2);
        var aSegment2 = new occ.createLine(aPnt2, aPnt3);
        var aSegment3 = new occ.createLine(aPnt3, aPnt4);
        var aSegment4 = new occ.createLine(aPnt4, aPnt1);

        var aWire  = new occ.Wire(aSegment1,aSegment2,aSegment3,aSegment4);
        aWire.isClosed.should.equal(true);
        aWire.numEdges.should.equal(4);
        aWire.numVertices.should.equal(4);

        // the vector to extrude the face along.
        var aVector = [  -2 , -2.0 , 10];
        var aFace = new occ.Face(aWire);

        var myBody = occ.makePrism(aFace,aVector);

        occ.writeSTEP("extrudedFace.step",[myBody]);

    });
});
