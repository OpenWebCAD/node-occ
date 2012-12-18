// shape factory
var occ = require('./occ');
var should = require('should');

exports.makeBottle = function () {

    var myWidth     =  50.0;
    var myThickness =  30.0;
    var myHeight    =  70.0;
    var myFiletRadius = myThickness/12.0;

    var  aPnt1 = [ -myWidth / 2. ,                0  , 0];
    var  aPnt2 = [ -myWidth / 2. , -myThickness / 4. , 0];
    var  aPnt3 = [            0  , -myThickness / 2. , 0];
    var  aPnt4 = [  myWidth / 2. , -myThickness / 4. , 0];
    var  aPnt5 = [  myWidth / 2. ,                0  , 0];

    var aSegment1 = new occ.Edge().createLine(aPnt1,aPnt2);
    var aArc1     = new occ.Edge().createArc3P(aPnt2,aPnt3,aPnt4);
    var aSegment2 = new occ.Edge().createLine(aPnt4,aPnt5);

    var aHalfWire  = new occ.Wire(aSegment1,aArc1,aSegment2);

    aHalfWire.isClosed.should.be.false;
    aHalfWire.numEdges.should.equal(3);
    aHalfWire.numVertices.should.equal(4);

    var trsf  = new occ.Transformation().makePlaneMirror([0,0,0],occ.XDir);

    var aMirroredWire = aHalfWire.transformed(trsf);
    aMirroredWire.isClosed.should.be.false;

    var aWire = new occ.Wire(aHalfWire,aMirroredWire);
    aWire.isClosed.should.be.true;

    // occ.makeFace(aWire);
    var aFace = new occ.Face(aWire);
    aFace.numWires.should.equal(1);

    var prism = new occ.makePrism(aFace,[0,0,myHeight]);

    // prism.fillet(prism.getEdges(),myFiletRadius);

    //TODO to be continued
    return prism;

}