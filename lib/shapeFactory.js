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

    var  aPnt1 = [ -myWidth / 2.0 ,                0.0 , 0];
    var  aPnt2 = [ -myWidth / 2.0 , -myThickness / 4.0 , 0];
    var  aPnt3 = [            0.0  , -myThickness / 2.0 , 0];
    var  aPnt4 = [  myWidth / 2.0 , -myThickness / 4.0 , 0];
    var  aPnt5 = [  myWidth / 2.0 ,                0.0 , 0];

    var aSegment1 = new occ.Edge().createLine(aPnt1,aPnt2);
    var aArc1     = new occ.Edge().createArc3P(aPnt2,aPnt3,aPnt4);
    var aSegment2 = new occ.Edge().createLine(aPnt4,aPnt5);

    var aHalfWire  = new occ.Wire(aSegment1,aArc1,aSegment2);

    aHalfWire.isClosed.should.equal(false);

    aHalfWire.numEdges.should.equal(3);
    aHalfWire.numVertices.should.equal(4);

    var trsf  = new occ.Transformation().makePlaneMirror([0,0,0],occ.YDir);

    var aMirroredWire = aHalfWire.transformed(trsf);
    aMirroredWire.isClosed.should.equal(false);

    var aWire = new occ.Wire(aHalfWire,aMirroredWire);
    aWire.isClosed.should.equal(true);

    // occ.makeFace(aWire);
    var aFace = new occ.Face(aWire);
    aFace.numWires.should.equal(1);

    var myBody = occ.makePrism(aFace,[0,0,myHeight]);
    //xx occ.writeSTEP("body1_a.step",myBody);


    myBody.fillet(myBody.getEdges(),myFilletRadius);
    //xx occ.writeSTEP("body1_b.step",myBody);

    //  --- create bottle neck

    var neckLocation = [ 0.0,0.0,myHeight];
    var neckAxis     = occ.ZDir;
    var neckRadius   = myThickness /  4.0 ;
    var neckHeight   = myHeight    / 10.0 ;

    var myNeck = occ.makeCylinder([neckLocation,neckAxis],neckRadius, neckHeight);

    myBody = occ.fuse(myBody,myNeck);

    //xx occ.writeSTEP("body1_c.step",myBody);


/*
    //  --- create an hollow solid
    var zMax =0;
    var faceToRemove = myBody.filterFace(
        function compare(face,data) {
            if (face.isPlane) {

            }
    });

    // return myBody;
 */
    return myBody;

};

exports.makePan = function(csg)
{
    "use strict";

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

};

exports.makeRoundedPlate = function(csg,R1,R2,L,thickness)
{
     "use strict";

    R1 = R1 || 7;
    R2 = R2 || 2.5;

    L  = L|| 12;
    thickness = thickness || 1.5;

    var rad2deg = 180/Math.atan(1.)/4.0;

    var sinAlpha = (R1-R2)/L;
    var angle = Math.asin(sinAlpha)*rad2deg;
    var L2    = L * (1-sinAlpha*sinAlpha);

    var q0 = [-200*R1,0,thickness];
    var p1 = [-R1,L2,0];
    var p2 = [0,0,thickness];
    var p3 = [R1,L2,0];
    var q3 = [200*R1,0,thickness];

    var a = csg.makeBox(p1,p2);

    a.rotate(p2,[0,0,1],-angle);

    var b = csg.makeBox(p2,p3);
    b.rotate(p2,[0,0,1],angle);
    var v = csg.fuse(b,a);

    // remove unwanted material
    v = csg.cut(v,csg.makeBox(q0,p1).rotate(p2,[0,0,1],-angle));
    v = csg.cut(v,csg.makeBox(q3,p3).rotate(p2,[0,0,1],angle));

//    return v;
    var c1 = csg.makeCylinder([0,0,0],[0,0,thickness],R1);
    v = csg.fuse(v,c1);
    var c2 = csg.makeCylinder([0,L,0],[0,L,thickness],R2);
    v = csg.fuse(v,c2);

    return v;
};

exports.makeRivetPlate = function(csg,params)
{
    // see http://www.tracepartsonline.net/%28S%281gpggj45ixmu5o5540hxuofo%29%29/PartsDefs/Production/ALCOA/22-02072002-063054/documents/AJAL103.pdf
    "use strict";
    // params = { A: 18,    B:7,    C:6.0,  F:4.7,  H:3.6,  H1:2.0,  H2:3.0,  J:6.0,  K:2.5,  R:2.5,  V:1.5 };
    // params = { A: 24.3,  B:9.5,  C:8.5,  F:4.7,  H:5.3,  H1:2.8,  H2:4.8,  J:8.0,  K:2.5,  R:3.0,  V:1.5 };
    // params = { A: 26.0,  B:11.0, C:9.5,  F:4.7,  H:6.0,  H1:3.5,  H2:5.5,  J:8.0,  K:2.5,  R:3.0,  V:1.5 };
    // params = { A: 29.0,  B:13.0, C:11.0, F:4.7,  H:7.0,  H1:4.2,  H2:6.2,  J:8.0,  K:3.3,  R:3.5,  V:1.5 };
    // params = { A: 33.5,  B:18.0, C:13.0, F:6.0,  H:9.3,  H1:5.1,  H2:8.5,  J:8.0,  K:3.3,  R:3.5,  V:1.5 };
    var A = params.A ||18;
    var B = params.B ||7;
    var C = params.C ||6;
    var F = params.F ||4.7;
    var H = params.H ||3.6;
    var H1 =params.H1 || 2.0;
    var H2 =params.H2 || 3.0;
    var J = params.J||6.0;
    var K = params.K ||2.5;
    var R = params.R || 2.5;
    var V = params.V ||1.5;

    var base = exports.makeRoundedPlate(csg,B/2,R,C+J,V);
    base = csg.fuse(base,csg.makeCylinder([0,0,0],[0,0,H1],B/2*0.8));
    base = csg.fuse(base,csg.makeCone([0,0,H1],[0,0,H2],B/2*0.8,B/2*0.6));
    base = csg.cut(base,csg.makeCylinder([0,0,0],[0,0,H2],K/2));
    base = csg.cut(base,csg.makeCylinder([0,C+J,0],[0,C+J,H2],K/2));
    base = csg.cut(base,csg.makeCylinder([0,C,0],[0,C,V],K/2));
    return base;
};


occ.makeRivetPlate  = function(params) {
    "use strict";
    return exports.makeRivetPlate(occ,params);
};

occ.makeRoundedPlate  = function(R1,R2,L,thickness) {
    "use strict";
    return exports.makeRoundedPlate(occ,R1,R2,L,thickness);
};

