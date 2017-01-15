// shape factory


const occ = require('./occ');
const should = require('should');

/**
 *
 * @param parameters
 * @param parameters.height = 70.0
 * @param parameters.filletRadius = 2.6
 * @returns {*}
 */
exports.makeBottle = function (occ, parameters)
{
    occ.hasOwnProperty("makeLine").should.eql(true);

    parameters = parameters || {};

    let smallThickness = 1.0;
    let myWidth = 50.0;
    let myThickness = 30.0;
    let myHeight = parameters.height || 70.0;
    let myFilletRadius = parameters.filletRadius || myThickness / 12.0;

    //
    //    (1)
    //     +.......................|.......................+ (5)
    //     |                       .                       |
    //     |                       |                       |
    //  (2)+                       .                       + (4)
    //                             |
    //                             +(3)
    //
    let  aPnt1 = [ -myWidth / 2.0 ,                 0.0 , 0];
    let  aPnt2 = [ -myWidth / 2.0 , -myThickness  / 4.0 , 0];
    let aPnt3 = [0.0, -myThickness / 2.0, 0];
    let  aPnt4 = [  myWidth / 2.0 , -myThickness  / 4.0 , 0];
    let  aPnt5 = [  myWidth / 2.0 ,                 0.0 , 0];

    let aSegment1 = occ.makeLine(aPnt1, aPnt2);
    let aArc1 = occ.makeArc3P(aPnt2, aPnt3, aPnt4);
    let aSegment2 = occ.makeLine(aPnt4, aPnt5);

    let aHalfWire = occ.makeWire(aSegment1, aArc1, aSegment2);

    aHalfWire.isClosed.should.equal(false);

    aHalfWire.numEdges.should.equal(3);
    aHalfWire.numVertices.should.equal(4);

    let trsf = occ.makePlaneMirror([0, 0, 0], [0, 1, 0]);

    let aMirroredWire = aHalfWire.transformed(trsf);
    aMirroredWire.isClosed.should.equal(false);

    let aWire = occ.makeWire(aHalfWire, aMirroredWire);

    aWire.isClosed.should.equal(true);

    let aFace = occ.makeFace(aWire);
    aFace.numWires.should.equal(1);

    let myBody = occ.makePrism(aFace,[0,0,myHeight]);

    myBody = occ.makeFillet(myBody,myBody.getEdges(),myFilletRadius);
    //xx occ.writeSTEP("body1_b.step",myBody);

    //  --- create bottle neck

    let neckLocation = [ 0.0,0.0,myHeight];
    let neckAxis = [0, 0, 1.0];
    let neckRadius   = myThickness /  4.0 ;
    let neckHeight   = myHeight    / 10.0 ;

    let myNeck = occ.makeCylinder([neckLocation,neckAxis],neckRadius, neckHeight);

    myBody = occ.fuse(myBody,myNeck);
    //xx occ.writeSTEP("body1_c.step",myBody);


    //  --- create an hollow solid
    let zMax =0;
    let faceToRemove ;
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

    let height = _height  || 20;
    let radius  = _radius || 25.0;
    let thickness = 1;
    let handleRadius = 4;
    let handleLength = 30;

    let s1 = csg.makeCylinder([0,0,0],[0,0,height] , radius);

    let s2 = csg.makeSphere([0,0,0], radius);

    let s3 = csg.makeCylinder([0,0,-radius*0.7],[0,0,height], radius*2);
    let s4 = csg.fuse(s1,s2);
    let s5 = csg.common(s4,s3);

    let handle = csg.makeCylinder(
        [radius-2*thickness,0,height-handleRadius*1.1],
        [handleLength+radius-2*thickness,0,height-handleRadius*1.1],
        handleRadius );

    let s6 = csg.fuse(s5,handle);

    let r1 = csg.makeCylinder([0,0,0],[0,0,height] , radius-thickness);
    let r2 = csg.makeSphere([0,0,0],radius-thickness);
    let r3 = csg.makeCylinder( [0,0,-radius*0.7+thickness],[0,0,height], radius*2);
    let r4 = csg.fuse(r1,r2);
    let r5 = csg.common(r4,r3);

    let body = csg.cut(s6,r5);

    let lidHeight = 10;
    let lid = exports.makePanLid(csg,radius,lidHeight,height);
    lid.translate([0,0,1]);

    body = csg.fuse(body,lid);

    return body;

};
exports.makePanLid = function(csg,_r,_height,_H) {
    "use strict";

    // r : pan radius
    // height :
    let r      = _r || 25.0;
    let h      = _height || 10;
    let thickness = 1;

    // r**2 + (R-h)**2 = R**2
    // r**2 + R**2-2*h*R +h**2 = R**2
    // => R = ( r**2+h**2)/(2*h);

    let R =  ( r*r+h*h)/(2*h);

    let center = [ 0, 0, _H + h -R ];

    let outerSphere = csg.makeSphere(center,R);
    let innerSphere = csg.makeSphere(center,R-thickness);
    let solid = csg.cut(outerSphere,innerSphere);

    let cyl  = csg.makeCylinder([0,0, _H + h -3*R], [0,0,_H], R+r*2);

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

    let rad2deg = 180/Math.atan(1.0)/4.0;

    let sinAlpha = (R1-R2)/L;
    let angle = Math.asin(sinAlpha)*rad2deg;
    let L2    = L * (1-sinAlpha*sinAlpha);

    let q0 = [-200*R1,0,thickness];
    let p1 = [-R1,L2,0];
    let p2 = [0,0,thickness];
    let p3 = [R1,L2,0];
    let q3 = [200*R1,0,thickness];

    let a = csg.makeBox(p1,p2);

    a.rotate(p2,[0,0,1],-angle);

    let b = csg.makeBox(p2,p3);
    b.rotate(p2,[0,0,1],angle);
    let v = csg.fuse(b,a);

    // remove unwanted material
    v = csg.cut(v,csg.makeBox(q0,p1).rotate(p2,[0,0,1],-angle));
    v = csg.cut(v,csg.makeBox(q3,p3).rotate(p2,[0,0,1],angle));

//    return v;
    let c1 = csg.makeCylinder([0,0,0],[0,0,thickness],R1);
    v = csg.fuse(v,c1);
    let c2 = csg.makeCylinder([0,L,0],[0,L,thickness],R2);
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
    let A = params.A ||18;
    let B = params.B ||7;
    let C = params.C ||6;
    let F = params.F ||4.7;
    let H = params.H ||3.6;
    let H1 =params.H1 || 2.0;
    let H2 =params.H2 || 3.0;
    let J = params.J||6.0;
    let K = params.K ||2.5;
    let R = params.R || 2.5;
    let V = params.V ||1.5;

    let base = exports.makeRoundedPlate(csg,B/2,R,C+J,V);
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
	let angle = Math.atan((r1-r2)/H);
	// calculate the distance below the cylinder top
	// at which the torus is tangent to the cylinder
	let d = - Math.tan(angle/2) * rs;
	// calculate the distance above the cone bottom
	// at which the torus is tangent to the cone
	let d2 = d + rs * Math.sin(angle);
	// calculate the radius of the cone at the 
	// tangent edge with the torus.
	let r1b = r1 -rs*(1.0-Math.cos(angle));

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
    
    let r0 = r -rs;
    let h0 = h -rs;
    let tanAlpha = h0/r0;
    let alpha = Math.atan(tanAlpha);
    
    let hyp0_2 = r0*r0 + h0*h0;
    // h0/hyp0 = (hyp0/2)/R0
    
    let R0 =   (hyp0_2/2.0)/h0;
    let R = R0+ rs;
    
    let center = [ 0, 0, h0-R0 ];


    let outerSphere = csg.makeSphere(center,R);
    let innerSphere = csg.makeSphere(center,R-thickness);
    let solid = csg.cut(outerSphere,innerSphere);
    
    // lets cut the sphere 
    let cuttingcone =  csg.makeCone(center,[0,0,1],2*alpha,R);
    solid = csg.common(solid,cuttingcone);
    
    let cyl = csg.makeCylinder(center,[0,0,0],R);
    // lets add a torus
    let outerfillet = csg.makeTorus([0,0,0],[0,0,1],r0,rs);
    outerfillet = csg.cut(outerfillet,cuttingcone);
    outerfillet = csg.cut(outerfillet,cyl);
    
    
    let fillet = outerfillet; 
    if (rs-thickness>0) {
        let innerfillet = csg.makeTorus([0,0,0],[0,0,1],r0,rs-thickness);
        innerfillet = csg.cut(innerfillet,cuttingcone);
        fillet = csg.cut(fillet,innerfillet);
    }
    fillet = csg.cut(fillet,cuttingcone);
    
    return csg.compound([solid,fillet]);
    
};
 
exports.makeTube = function (csg,p1,p2,R,thickness) {
    let cyl1 = csg.makeCylinder(p1,p2,R);
    let cyl2 = csg.makeCylinder(p1,p2,R-thickness);
    return csg.cut(cyl1,cyl2);
};


exports.makeHollowCylinder = function (csg,R,H,h,rf,thickness) {
    let top =  exports.makeLidWithTorus(csg,R,h,rf,thickness);
    let bottom = top.clone();
    bottom = bottom.rotate([0,0,0],[1,0,0],180);
    
    let cyl = exports.makeTube(csg,[0,0,0],[0,0,H],R,thickness);
    top = top.translate([0,0,H]);
    let solid =  csg.fuse(bottom,cyl);
    solid =  csg.fuse(solid,top);
    return solid;
};

    
exports.testHollowCylinder= function(csg) {

  let obj =  makeHollowCylinder(csg,40,100,10,5,1);
  // create a section to verify visually the correctness of
  // the construction.
  let cuttingPlanes = csg.makeBox([0,0,-100],[100,200,100]);
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
      

    let u = 1.6; // lego unit

    let outerWidth   = nX*u*5;
    let outerLength  = nY*u*5;
    let outerHeight  = h *u;
    
    // console.log(" outerWidth = ",outerWidth);
    // console.log(" outerLength = ",outerLength);
    // console.log(" outerHeight = ",outerHeight);

    let outerBlock = csg.makeBox([0,0,0],[outerWidth,outerLength,outerHeight]);
    
    let innerWidth = outerWidth-2*u;
    let innerLength =  outerLength-2*u;
    let innerHeight = outerHeight-u;
    
    // console.log(" innerWidth = ",innerWidth);
    // console.log(" innerLength = ",innerLength);
    // console.log(" innerHeight = ",innerHeight);  
    
    let innerBlock = csg.makeBox([0,0,0],[innerWidth,innerLength,innerHeight]);
    innerBlock = innerBlock.translate([u,u,0]);
    let hollowBlock = csg.cut(outerBlock,innerBlock);
    let h1 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight-3*u],
                [2.5*u,2.5*u,outerHeight+3*u],
                0.75*u);

    let h2 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight],
                [2.5*u,2.5*u,outerHeight+u],
                1.5*u);

    // installer la grille
    let x=0,y=0;
    for (;y<nY;y++) {
        let hh1 = h1.clone();
        let hh2 = h2.clone();
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
    
    let pinOuter  = csg.makeCylinder(
                [2.5*u,2.5*u,0],
                [2.5*u,2.5*u,outerHeight-u],
                u
        );
    let pinInner  = csg.makeCylinder(
                [2.5*u,2.5*u,0],
                [2.5*u,2.5*u,outerHeight-u],
                0.5*u
        );
    let pin = csg.cut(pinOuter,pinInner);
    let p,pp;
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


let makeRepetition = function(csg,shape,dX,nX,dY,nY) {
    let h1 = shape.clone() ;
     // installer la grille
    let shapeArray = [];
    let x=0,y=0;
    for (;y<nY;y++) {
        let hh1 = h1.clone();
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
      

    let u = 1.6; // lego unit

    let outerWidth   = nX*u*5;
    let outerLength  = nY*u*5;
    let outerHeight  = h *u;
    
    let brick = csg.makeBox([0,0,0],[outerWidth,outerLength,outerHeight]);
    brick = csg.makeThickSolid(brick,brick.faces.bottom,-u);
    
    let h2 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight],
                [2.5*u,2.5*u,outerHeight+u],
                1.5*u);
    
    let tetons = makeRepetition(csg,h2,5*u,nX,5*u,nY);
    brick = csg.fuse(brick,tetons);   
    
    let h1 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight-3*u],
                [2.5*u,2.5*u,outerHeight+0.75],
                0.74*u);   
    tetons = makeRepetition(csg,h1,5*u,nX,5*u,nY);  
    brick = csg.cut(brick,tetons);

    //xx console.log(Object.keys(brick.faces));//.bottom);
   
   // small pins
   let pinOuter  = csg.makeCylinder(
            [2.5*u,2.5*u,0],
            [2.5*u,2.5*u,outerHeight-u],
            u
    );
    let pinInner  = csg.makeCylinder(
            [2.5*u,2.5*u,0],
            [2.5*u,2.5*u,outerHeight-u],
            0.5*u
    );
    
    let pin = csg.cut(pinOuter,pinInner); 
  
    let p;
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

    let w = 65;
    let r = 20;
    let h = 12;

    let p0 = occ.makeVertex([0, 0, 0]);

    let p1 = p0.translate([-w / 2.0, 0, 0]);

    let p2 = p1.translate([0.0, h, 0]);
    let p3 = p0.translate([-r, h, 0]);
    let p4 = p0.translate([0, h + r, 0]);


    let trsf = occ.makePlaneMirror([0, 0, 0], [1, 0, 0]);

    let q1 = p1.transformed(trsf);
    let q2 = p2.transformed(trsf);
    let q3 = p3.transformed(trsf);

    let e1 = occ.makeLine(q1, p1);
    let e2 = occ.makeLine(p1, p2);
    let e3 = occ.makeLine(p2, p3);
    let e4 = occ.makeArc3P(p3, p4, q3);
    let e5 = occ.makeLine(q3, q2);
    let e6 = occ.makeLine(q2, q1);

    let wire1 = occ.makeWire(e1, e2, e3, e4, e5, e6);
    wire1.numEdges.should.equal(6);

    let face1 = occ.makeFace(wire1);

    let height = 12;
    let body1 = occ.makePrism(face1, [0, 0, height]);

    return body1;
};

exports.makeTutorialPart = function (occ) {

    let w = 60;
    let H = 50;
    let h = H / 2;
    let a = 7.5;
    let b = 20;

    let p0 = [b, -a / 2, 0];
    let p1 = [0, -a / 2, 0];
    let p2 = [0, -h, 0];
    let p3 = [w, -h, 0];
    let p4 = [w, h, 0];
    let p5 = [0, h, 0];
    let p6 = [0, a / 2, 0];
    let p7 = [b, a / 2, 0];

    let p8 = [b + a / 2, 0, 0];

    let e1 = occ.makeLine(p0, p1);
    let e2 = occ.makeLine(p1, p2);
    let e3 = occ.makeLine(p2, p3);
    let e4 = occ.makeLine(p3, p4);
    let e5 = occ.makeLine(p4, p5);
    let e6 = occ.makeLine(p5, p6);
    let e7 = occ.makeLine(p6, p7);
    let e8 = occ.makeArc3P(p7, p8, p0);

    let wire = occ.makeWire(e1, e2, e3, e4, e5, e6, e7, e8);
    wire.isClosed.should.eql(true);
    console.log(" wire Length = ", wire.length);

    let height = 20;
    let face = occ.makeFace(wire);
    let body1 = occ.makePrism(face, [0, 0, height]);

    // --------------------------------------------------
    let height2 = 45;
    let circle = occ.makeCircle([w, 0, 0], [0, 0, 1], h);
    let wire2 = occ.makeWire(circle);
    let face2 = occ.makeFace(wire2);
    let body2 = occ.makePrism(face2, [0, 0, height2]);

    // -----------------------------------------------------
    let body3 = occ.fuse(body1, body2);

    //
    //     ------+
    //   /
    // +
    //    ------+
    //
    let R = 15;
    let c = 7.5;
    let angle = Math.asin(7.5 / R);
    let dx = R * Math.cos(angle);
    let dy = R * Math.sin(angle);

    let q1 = [w + dx, dy, 0];
    let q2 = [w + dx + 100, dy, 0];
    let q3 = [w + dx + 100, -dy, 0];
    let q4 = [w + dx, -dy, 0];
    let q5 = [w - R, 0, 0];

    let ee1 = occ.makeLine(q1, q2);
    let ee2 = occ.makeLine(q2, q3);
    let ee3 = occ.makeLine(q3, q4);
    let ee4 = occ.makeArc3P(q4, q5, q1);
    let wire4 = occ.makeWire(ee1, ee2, ee3, ee4);
    let face4 = occ.makeFace(wire4);
    let body4 = occ.makePrism(face4, [0, 0, height2]);

    let body5 = occ.cut(body3, body4);


    let edges = body5.getEdges();
    // --------------------------------------------
    // Select vertical edges with vertex P1 and P6
    // --------------------------------------------
    function same(a, b, tol) {
        return Math.abs(a - b) < tol;
    }

    function samePoint(p1, p2) {
        let tol = 0.001;
        return same(p1.x, p2.x, tol) &&
            same(p1.y, p2.y, tol) &&
            same(p1.z, p2.z, tol);
    }

    function selectEdge(edges, p) {

        if (p instanceof occ.Vertex) {
            p = occ.makeVertex(p)
        }
        let results = edges.filter(function (edge) {
            let firstVertex = edge.firstVertex;
            let lastVertex = edge.lastVertex;
            return ( samePoint(firstVertex, p) || samePoint(lastVertex, p)) &&
                same(firstVertex.x, lastVertex.x, 0.01) &&
                same(firstVertex.y, lastVertex.y, 0.01)
                ;
        });
        return results[0];
    }

    let edges_for_filet = [selectEdge(edges, p2), selectEdge(edges, p5)];

    let body6 = occ.makeFillet(body5, edges_for_filet, 10);

    // create hole
    let smallR = 5;
    let heigth3 = height2 - smallR - 10;
    let cyl = occ.makeCylinder([w - R - 10, 0, heigth3], [w - R + 20, 0, heigth3], smallR);

    let body7 = occ.cut(body6, cyl);

    return body7;
};

