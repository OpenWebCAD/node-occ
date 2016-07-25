// shape factory


var occ = require('./occ');
var should = require('should');

var exports = exports || {};

/**
 *
 * @param parameters
 * @param parameters.height = 70.0
 * @param parameters.filletRadius = 2.6
 * @returns {*}
 */
exports.makeBottle = function (occ, parameters)
{
    occ.hasOwnProperty("createLine").should.eql(true);

    parameters = parameters || {};

    var smallThickness = 1.0;
    var myWidth = 50.0;
    var myThickness = 30.0;
    var myHeight = parameters.height || 70.0;
    var myFilletRadius = parameters.filletRadius || myThickness / 12.0;

    //
    //    (1)
    //     +.......................|.......................+ (5)
    //     |                       .                       |
    //     |                       |                       |
    //  (2)+                       .                       + (4)
    //                             |
    //                             +(3)
    //
    var  aPnt1 = [ -myWidth / 2.0 ,                 0.0 , 0];
    var  aPnt2 = [ -myWidth / 2.0 , -myThickness  / 4.0 , 0];
    var aPnt3 = [0.0, -myThickness / 2.0, 0];
    var  aPnt4 = [  myWidth / 2.0 , -myThickness  / 4.0 , 0];
    var  aPnt5 = [  myWidth / 2.0 ,                 0.0 , 0];

    var aSegment1 = occ.createLine(aPnt1, aPnt2);
    var aArc1 = occ.createArc3P(aPnt2, aPnt3, aPnt4);
    var aSegment2 = occ.createLine(aPnt4, aPnt5);

    var aHalfWire = occ.makeWire(aSegment1, aArc1, aSegment2);

    aHalfWire.isClosed.should.equal(false);

    aHalfWire.numEdges.should.equal(3);
    aHalfWire.numVertices.should.equal(4);

    var trsf = occ.makePlaneMirror([0, 0, 0], [0, 1, 0]);

    var aMirroredWire = aHalfWire.transformed(trsf);
    aMirroredWire.isClosed.should.equal(false);

    var aWire = occ.makeWire(aHalfWire, aMirroredWire);

    aWire.isClosed.should.equal(true);

    var aFace = occ.makeFace(aWire);
    aFace.numWires.should.equal(1);

    var myBody = occ.makePrism(aFace,[0,0,myHeight]);

    myBody = occ.makeFillet(myBody,myBody.getEdges(),myFilletRadius);
    //xx occ.writeSTEP("body1_b.step",myBody);

    //  --- create bottle neck

    var neckLocation = [ 0.0,0.0,myHeight];
    var neckAxis = [0, 0, 1.0];
    var neckRadius   = myThickness /  4.0 ;
    var neckHeight   = myHeight    / 10.0 ;

    var myNeck = occ.makeCylinder([neckLocation,neckAxis],neckRadius, neckHeight);

    myBody = occ.fuse(myBody,myNeck);
    //xx occ.writeSTEP("body1_c.step",myBody);


    //  --- create an hollow solid
    var zMax =0;
    var faceToRemove ;
    myBody.getFaces().forEach(function(face){
        //xx console.log(" examining  face = ", myBody.getShapeName(face),face.isPlanar,face.centreOfMass.z);
        if (face.isPlanar) {
           if (face.centreOfMass.z >= zMax) {
                faceToRemove = face;
                zMax = face.centreOfMass.z;
           }
        }        
    });

    myBody = occ.makeThickSolid(myBody,faceToRemove,smallThickness);
    return myBody;

};

exports.makePan = function(csg,_height,_radius)
{
    "use strict";

    var height = _height  || 20;
    var radius  = _radius || 25.0;
    var thickness = 1;
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

    var lidHeight = 10;
    var lid = exports.makePanLid(csg,radius,lidHeight,height);
    lid.translate([0,0,1]);

    body = csg.fuse(body,lid);

    return body;

};
exports.makePanLid = function(csg,_r,_height,_H) {
    "use strict";

    // r : pan radius
    // height :
    var r      = _r || 25.0;
    var h      = _height || 10;
    var thickness = 1;

    // r**2 + (R-h)**2 = R**2
    // r**2 + R**2-2*h*R +h**2 = R**2
    // => R = ( r**2+h**2)/(2*h);

    var R =  ( r*r+h*h)/(2*h);

    var center = [ 0, 0, _H + h -R ];

    var outerSphere = csg.makeSphere(center,R);
    var innerSphere = csg.makeSphere(center,R-thickness);
    var solid = csg.cut(outerSphere,innerSphere);

    var cyl  = csg.makeCylinder([0,0, _H + h -3*R], [0,0,_H], R+r*2);

    solid = csg.cut(solid,cyl);

    return solid;

};

exports.makeRoundedPlate = function(csg,R1,R2,L,thickness)
{
     "use strict";

    R1 = R1 || 7;
    R2 = R2 || 2.5;

    L  = L|| 12;
    thickness = thickness || 1.5;

    var rad2deg = 180/Math.atan(1.0)/4.0;

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
    base = csg.fuse(base,csg.makeCone([0,0,H1],B/2*0.8,[0,0,H2],B/2*0.6));
    base = csg.cut(base,csg.makeCylinder([0,0,0],[0,0,H2],K/2));
    base = csg.cut(base,csg.makeCylinder([0,C+J,0],[0,C+J,H2],K/2));
    base = csg.cut(base,csg.makeCylinder([0,C,0],[0,C,V],K/2));
    return base;
};

occ.makePan  = function(params) {
    "use strict";
    // { R: 10, H: 30 }
    return exports.makePan(occ,params.H,params.R);
};
occ.makeRivetPlate  = function(params) {
    "use strict";
    return exports.makeRivetPlate(occ,params);
};

occ.makeRoundedPlate  = function(R1,R2,L,thickness) {
    "use strict";
    return exports.makeRoundedPlate(occ,R1,R2,L,thickness);
};

/**
 * params:
 *   r1 : radius of the cylinder and base of cone
 *   r2 : cone top face radius 
 *   H  : cone height
 *   H2 : cylinder height
 *   rs : fillet radius or radius of the torus which is
 *         tangent to the cone and the cylinder.
 */
occ.makeCylinderTorusCone = function(r1,r2,H,H2,rs)
{
	// ------------------------------------------------------------
	// create a cylinder capped with a cone and a round fillet
	// at the intersection.
	//              R2|
	//           /----. H  
	//          /     |    
	// (rs)    (------.- 0  ( torus)
	//         |      |
	//         |      .  
	//         +------|- -H2
	//              R1
	// ------------------------------------------------------------

	/* example : r1 = 20 ,r2 = 15 , H  = 5 ,H2 = 10 rs = 5  */
	
	// calculate the cone half angle
	var angle = Math.atan((r1-r2)/H);
	// calculate the distance below the cylinder top
	// at which the torus is tangent to the cylinder
	var d = - Math.tan(angle/2) * rs;
	// calculate the distance above the cone bottom
	// at which the torus is tangent to the cone
	var d2 = d + rs * Math.sin(angle);
	// calculate the radius of the cone at the 
	// tangent edge with the torus.
	var r1b = r1 -rs*(1.0-Math.cos(angle));

	cyl   = csg.makeCylinder([0,0,-H2],[0,0,d],r1);
	cone  = csg.makeCone([0,0,d2],r1b,[0,0,H],r2);
	tore  = csg.makeTorus([0,0,d],[0,0,1],r1-rs,rs);
	
	v  = csg.cut(tore,cone);
	v  = csg.cut(v,cyl);

	v = csg.fuse(v,cone);
	v = csg.fuse(v,cyl);
	return v; 
};


/**
 * parameters:
 *   r      : external radius of the lid
 *   height :  
 *   H      : Z position of the planar face of the lid
 *                      
 *                 _ ----|
 *               +       .
 *             /  \/     |
 *             +--+\     .
 *                  \    |
 *                   \   .
 *                    \  |
 *                     \ |
 *                      \|
 *                       +
 */
exports.makeLidWithTorus = function(csg,r,h,rs,thickness) {
    "use strict";
    
    var r0 = r -rs;
    var h0 = h -rs;
    var tanAlpha = h0/r0;
    var alpha = Math.atan(tanAlpha);
    
    var hyp0_2 = r0*r0 + h0*h0;
    // h0/hyp0 = (hyp0/2)/R0
    
    var R0 =   (hyp0_2/2.0)/h0;
    var R = R0+ rs;
    
    var center = [ 0, 0, h0-R0 ];


    var outerSphere = csg.makeSphere(center,R);
    var innerSphere = csg.makeSphere(center,R-thickness);
    var solid = csg.cut(outerSphere,innerSphere);
    
    // lets cut the sphere 
    var cuttingcone =  csg.makeCone(center,[0,0,1],2*alpha,R);
    solid = csg.common(solid,cuttingcone);
    
    var cyl = csg.makeCylinder(center,[0,0,0],R);
    // lets add a torus
    var outerfillet = csg.makeTorus([0,0,0],[0,0,1],r0,rs);
    outerfillet = csg.cut(outerfillet,cuttingcone);
    outerfillet = csg.cut(outerfillet,cyl);
    
    
    var fillet = outerfillet; 
    if (rs-thickness>0) {
        var innerfillet = csg.makeTorus([0,0,0],[0,0,1],r0,rs-thickness);
        innerfillet = csg.cut(innerfillet,cuttingcone);
        fillet = csg.cut(fillet,innerfillet);
    }
    fillet = csg.cut(fillet,cuttingcone);
    
    return csg.compound([solid,fillet]);
    
};
 
exports.makeTube = function (csg,p1,p2,R,thickness) {
    var cyl1 = csg.makeCylinder(p1,p2,R);
    var cyl2 = csg.makeCylinder(p1,p2,R-thickness);
    return csg.cut(cyl1,cyl2);
};


exports.makeHollowCylinder = function (csg,R,H,h,rf,thickness) {
    var top =  exports.makeLidWithTorus(csg,R,h,rf,thickness);
    var bottom = top.clone();
    bottom = bottom.rotate([0,0,0],[1,0,0],180);
    
    var cyl = exports.makeTube(csg,[0,0,0],[0,0,H],R,thickness);
    top = top.translate([0,0,H]);
    var solid =  csg.fuse(bottom,cyl);
    solid =  csg.fuse(solid,top);
    return solid;
};

    
exports.testHollowCylinder= function(csg) {

  var obj =  makeHollowCylinder(csg,40,100,10,5,1);
  // create a section to verify visually the correctness of
  // the construction.
  var cuttingPlanes = csg.makeBox([0,0,-100],[100,200,100]);
  return csg.cut(obj,cuttingPlanes);

};

 
exports.makeLegoBrickSlow = function(csg,nX,nY,h) {

    "use strict";
    if (h === 'thin') {
        h=2;
    } else if (h ==='thick') {
        h=6;
    } else {
      throw new Error("'invalid h");
    }
      

    var u = 1.6; // lego unit

    var outerWidth   = nX*u*5;
    var outerLength  = nY*u*5;
    var outerHeight  = h *u;
    
    // console.log(" outerWidth = ",outerWidth);
    // console.log(" outerLength = ",outerLength);
    // console.log(" outerHeight = ",outerHeight);

    var outerBlock = csg.makeBox([0,0,0],[outerWidth,outerLength,outerHeight]);
    
    var innerWidth = outerWidth-2*u;
    var innerLength =  outerLength-2*u;
    var innerHeight = outerHeight-u;
    
    // console.log(" innerWidth = ",innerWidth);
    // console.log(" innerLength = ",innerLength);
    // console.log(" innerHeight = ",innerHeight);  
    
    var innerBlock = csg.makeBox([0,0,0],[innerWidth,innerLength,innerHeight]);
    innerBlock = innerBlock.translate([u,u,0]);
    var hollowBlock = csg.cut(outerBlock,innerBlock);
    var h1 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight-3*u],
                [2.5*u,2.5*u,outerHeight+3*u],
                0.75*u);

    var h2 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight],
                [2.5*u,2.5*u,outerHeight+u],
                1.5*u);

    // installer la grille
    var x=0,y=0;
    for (;y<nY;y++) {
        var hh1 = h1.clone();
        var hh2 = h2.clone();
        for (x=0;x<nX;x++) {
            //
            hollowBlock = csg.cut(hollowBlock,hh1);
            hollowBlock = csg.fuse(hollowBlock,hh2);
            hh1 = hh1.translate([5*u,0,0]);
            hh2 = hh2.translate([5*u,0,0]);
        }   
        h1 = h1.translate([0,5*u,0]);
        h2 = h2.translate([0,5*u,0]);
    }
    
    var pinOuter  = csg.makeCylinder(
                [2.5*u,2.5*u,0],
                [2.5*u,2.5*u,outerHeight-u],
                u
        );
    var pinInner  = csg.makeCylinder(
                [2.5*u,2.5*u,0],
                [2.5*u,2.5*u,outerHeight-u],
                0.5*u
        );
    var pin = csg.cut(pinOuter,pinInner);
    var p,pp;
    if (nY == 1 ) {
        // install small pin insid
        p = pin.clone();
        p =p.translate([2.5*u,0,0]);
        for (x=0;x<nX-1;x++) {
            hollowBlock = csg.fuse(hollowBlock,p);
            p =p.translate([5*u,0,0]);
        }
    }
    if (nX == 1 ) {   
        p = pin.clone();
        p =p.translate([0,2.5*u,0]);
        for (y=0;y<nY-1;y++) {
            hollowBlock = csg.fuse(hollowBlock,p);
            p =p.translate([0,5*u,0]);
        }   
    }
    if (nX > 1 && nY > 1) {
        
         pinOuter  = csg.makeCylinder(
                [5*u,5*u,0],
                [5*u,5*u,outerHeight-u],
                4.07/2.0*u
        );
        pinInner  = csg.makeCylinder(
                [5*u,5*u,0],
                [5*u,5*u,outerHeight-u],
                1.5*u
        );
        pin = csg.cut(pinOuter,pinInner);
        
        for (x=0;x<nX-1;x++) {
             pp = pin.clone();
             for (y=0;y<nY-1;y++) {
                hollowBlock = csg.fuse(hollowBlock,pp);
                pp = pp.translate([0,5*u,0]);
             }
             pin = pin.translate([5*u,0,0]);
         }
    }
    return hollowBlock;
};


var makeRepetition = function(csg,shape,dX,nX,dY,nY) {
    var h1 = shape.clone() ;
     // installer la grille
    var shapeArray = [];
    var x=0,y=0;
    for (;y<nY;y++) {
        var hh1 = h1.clone();
        for (x=0;x<nX;x++) {                
            shapeArray.push(hh1);
            hh1 = hh1.translate([dX,0,0]);
        }    
        h1 = h1.translate([0,dY,0]);
    }  
    return csg.compound(shapeArray);
};

exports.makeLegoBrick = function(csg,nX,nY,h) {

    "use strict";
    
    if (h === 'thin') {
        h=2;
    } else if (h ==='thick') {
        h=6;
    } else {
      throw new Error("'invalid h");
    }
      

    var u = 1.6; // lego unit

    var outerWidth   = nX*u*5;
    var outerLength  = nY*u*5;
    var outerHeight  = h *u;
    
    var brick = csg.makeBox([0,0,0],[outerWidth,outerLength,outerHeight]);
    brick = csg.makeThickSolid(brick,brick.faces.bottom,-u);
    
    var h2 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight],
                [2.5*u,2.5*u,outerHeight+u],
                1.5*u);
    
    var tetons = makeRepetition(csg,h2,5*u,nX,5*u,nY);
    brick = csg.fuse(brick,tetons);   
    
    var h1 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight-3*u],
                [2.5*u,2.5*u,outerHeight+0.75],
                0.74*u);   
    tetons = makeRepetition(csg,h1,5*u,nX,5*u,nY);  
    brick = csg.cut(brick,tetons);

    //xx console.log(Object.keys(brick.faces));//.bottom);
   
   // small pins
   var pinOuter  = csg.makeCylinder(
            [2.5*u,2.5*u,0],
            [2.5*u,2.5*u,outerHeight-u],
            u
    );
    var pinInner  = csg.makeCylinder(
            [2.5*u,2.5*u,0],
            [2.5*u,2.5*u,outerHeight-u],
            0.5*u
    );
    
    var pin = csg.cut(pinOuter,pinInner); 
  
    var p;
    if (nY == 1 ) {
        // install small pin insid
        p = pin.clone();
        p =p.translate([2.5*u,0,0]);
        tetons = makeRepetition(csg,p,5*u,nX-1,0,1);  
        brick = csg.fuse(brick,tetons);
        
    } else  if (nX == 1 ) {   
        
        p = pin.clone();
        p =p.translate([0,2.5*u,0]);        
        tetons = makeRepetition(csg,p,0,1,5*u,nY-1);
        brick = csg.fuse(brick,tetons);
        
    }  else if (nX > 1 && nY > 1) {        
        pinOuter  = csg.makeCylinder( [5*u,5*u,0], [5*u,5*u,outerHeight-u],4.07/2.0*u);
        pinInner  = csg.makeCylinder( [5*u,5*u,0],[5*u,5*u,outerHeight-u],1.5*u);
        pin = csg.cut(pinOuter,pinInner);        
        tetons = makeRepetition(csg,pin,5*u,nX-1,5*u,nY-1);
        brick = csg.fuse(brick,tetons);
    }
    
    return brick;
};


exports.makePiston = function (occ) {

// create the top wire
//             ---|
//           .    .
//         /      |
// 2+-----+       .
//  |     3       |
// 1+-------------.
//  |<------------w--...>
//

    var w = 65;
    var r = 20;
    var h = 12;

    var p0 = occ.makeVertex([0, 0, 0]);

    var p1 = p0.translate([-w / 2.0, 0, 0]);

    var p2 = p1.translate([0.0, h, 0]);
    var p3 = p0.translate([-r, h, 0]);
    var p4 = p0.translate([0, h + r, 0]);


    var trsf = occ.makePlaneMirror([0, 0, 0], [1, 0, 0]);

    var q1 = p1.transformed(trsf);
    var q2 = p2.transformed(trsf);
    var q3 = p3.transformed(trsf);

    var e1 = occ.createLine(q1, p1);
    var e2 = occ.createLine(p1, p2);
    var e3 = occ.createLine(p2, p3);
    var e4 = occ.createArc3P(p3, p4, q3);
    var e5 = occ.createLine(q3, q2);
    var e6 = occ.createLine(q2, q1);

    var wire1 = occ.makeWire(e1, e2, e3, e4, e5, e6);
    wire1.numEdges.should.equal(6);

    var face1 = occ.makeFace(wire1);

    var height = 12;
    var body1 = occ.makePrism(face1, [0, 0, height]);

    return body1;
};
