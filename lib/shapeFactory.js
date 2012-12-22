// shape factory
var occ = require('./occ');
var should = require('should');

var CSGBuilder = require("./CSGBuilder.js");
var CSGTree = require("./CSGTree.js");

exports.makeBottle = function ()
{

    var myWidth     =  50.0;
    var myThickness =  30.0;
    var myHeight    =  70.0;
    var myFilletRadius = myThickness/12.0;

    var  aPnt1 = [ -myWidth / 2. ,                0. , 0];
    var  aPnt2 = [ -myWidth / 2. , -myThickness / 4. , 0];
    var  aPnt3 = [            0  , -myThickness / 2. , 0];
    var  aPnt4 = [  myWidth / 2. , -myThickness / 4. , 0];
    var  aPnt5 = [  myWidth / 2. ,                0. , 0];

    var aSegment1 = new occ.Edge().createLine(aPnt1,aPnt2);
    var aArc1     = new occ.Edge().createArc3P(aPnt2,aPnt3,aPnt4);
    var aSegment2 = new occ.Edge().createLine(aPnt4,aPnt5);

    var aHalfWire  = new occ.Wire(aSegment1,aArc1,aSegment2);

    aHalfWire.isClosed.should.be.false;
    aHalfWire.numEdges.should.equal(3);
    aHalfWire.numVertices.should.equal(4);

    var trsf  = new occ.Transformation().makePlaneMirror([0,0,0],occ.YDir);

    var aMirroredWire = aHalfWire.transformed(trsf);
    aMirroredWire.isClosed.should.be.false;

    var aWire = new occ.Wire(aHalfWire,aMirroredWire);
    aWire.isClosed.should.be.true;

    // occ.makeFace(aWire);
    var aFace = new occ.Face(aWire);
    aFace.numWires.should.equal(1);

    var myBody = occ.makePrism(aFace,[0,0,myHeight]);
    //xx occ.writeSTEP("body1_a.step",myBody);


    myBody.fillet(myBody.getEdges(),myFilletRadius);
    //xx occ.writeSTEP("body1_b.step",myBody);

    //  --- create bottle neck

    var neckLocation = [ 0.,0.,myHeight];
    var neckAxis     = occ.ZDir;
    var neckRadius   = myThickness /  4.0 ;
    var neckHeight   = myHeight    / 10.0 ;

    var myNeck = occ.makeCylinder([neckLocation,neckAxis],neckRadius, neckHeight);

    myBody = occ.fuse(myBody,myNeck);

    //xx occ.writeSTEP("body1_c.step",myBody);

    return myBody;


    //  --- create an hollow solid
    var zMax =0;
    var faceToRemove = myBody.filterFace(
        function compare(face,data) {
            if (face.isPlane) {


            }
    });

    body.makeThickSolid([faceToRemove], -myThickness/50.0 , 1e-3 );

    return myBody;

}

exports.makePan = function(csg)
{

    var height = 20;
    var thickness = 1;
    var radius  =25.0;
    var handleRadius = 4;
    var handleLength = 30;

    var s1 = csg.makeCylinder([0,0,0],[0,0,height] , radius);

    var s2 = csg.makeSphere([0,0,0], radius);

    var s3 = csg.makeCylinder([0,0,-radius*0.7],[0,0,height], radius*2);
    var s4 = csg.fuse(s1,s2);
    var s5 = csg.common(s4,s3);

    var handle = csg.makeCylinder(
        [radius-2*thickness,0,height-handleRadius*1.1],
        [handleLength+radius-2*thickness,0,height-handleRadius*1.1],
        handleRadius );

    var s6 = csg.fuse(s5,handle);

    var r1 = csg.makeCylinder([0,0,0],[0,0,height] , radius-thickness);
    var r2 = csg.makeSphere([0,0,0],radius-thickness);
    var r3 = csg.makeCylinder( [0,0,-radius*0.7+thickness],[0,0,height], radius*2);
    var r4 = csg.fuse(r1,r2);
    var r5 = csg.common(r4,r3);

    var body = csg.cut(s6,r5);

    return body;

}

