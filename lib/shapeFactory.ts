import assert from "assert";

import {
  OCC,
  IEdge,
  IFace,
  IPoint,
  PointLike,
  Triplet,
  UInteger,
  VectorLike,
} from "./interfaces";
import { _occ } from "./occ";

export interface MakeBottleOptions {
  height: number; // @default 70
  filletRadius: number;
}

export function makeBottle(occ: OCC, parameters: MakeBottleOptions) {
  assert(occ.hasOwnProperty("makeLine"));

  parameters = parameters || {};

  const smallThickness = 1.0;
  const myWidth = 50.0;
  const myThickness = 30.0;
  const myHeight = parameters.height || 70.0;
  const myFilletRadius = parameters.filletRadius || myThickness / 12.0;

  //
  //    (1)
  //     +.......................|.......................+ (5)
  //     |                       .                       |
  //     |                       |                       |
  //  (2)+                       .                       + (4)
  //                             |
  //                             +(3)
  //
  const aPnt1: Triplet = [-myWidth / 2.0, 0.0, 0];
  const aPnt2: Triplet = [-myWidth / 2.0, -myThickness / 4.0, 0];
  const aPnt3: Triplet = [0.0, -myThickness / 2.0, 0];
  const aPnt4: Triplet = [myWidth / 2.0, -myThickness / 4.0, 0];
  const aPnt5: Triplet = [myWidth / 2.0, 0.0, 0];

  const aSegment1 = occ.makeLine(aPnt1, aPnt2);
  const aArc1 = occ.makeArc3P(aPnt2, aPnt3, aPnt4);
  const aSegment2 = occ.makeLine(aPnt4, aPnt5);

  const aHalfWire = occ.makeWire(aSegment1, aArc1, aSegment2);

  assert(false === aHalfWire.isClosed);

  assert(3 === aHalfWire.numEdges);
  assert(4 === aHalfWire.numVertices);

  const trsf = occ.makePlaneMirror([0, 0, 0], [0, 1, 0]);

  const aMirroredWire = aHalfWire.transformed(trsf);
  assert(false === aMirroredWire.isClosed);

  const aWire = occ.makeWire(aHalfWire, aMirroredWire);

  assert(aWire.isClosed);

  const aFace = occ.makeFace(aWire);
  assert(1 === aFace.numWires);

  let myBody = occ.makePrism(aFace, [0, 0, myHeight]);

  myBody = occ.makeFillet(myBody, myBody.getEdges(), myFilletRadius);
  //xx occ.writeSTEP("body1_b.step",myBody);

  //  --- create bottle neck

  const neckLocation: PointLike = [0.0, 0.0, myHeight];
  const neckAxis: VectorLike = [0, 0, 1.0];
  const neckRadius = myThickness / 4.0;
  const neckHeight = myHeight / 10.0;

  const myNeck = occ.makeCylinder(
    [neckLocation, neckAxis],
    neckRadius,
    neckHeight
  );

  myBody = occ.fuse(myBody, myNeck);
  //xx occ.writeSTEP("body1_c.step",myBody);

  //  --- create an hollow solid
  let zMax = 0;
  let faceToRemove: IFace | undefined;
  myBody.getFaces().forEach(function (face) {
    //xx console.log(" examining  face = ", myBody.getShapeName(face),face.isPlanar,face.centreOfMass.z);
    if (face.isPlanar && face.centreOfMass.z >= zMax) {
      faceToRemove = face;
      zMax = face.centreOfMass.z;
    }
  });

  if (!faceToRemove) throw new Error("Cannot find face to remove");

  myBody = occ.makeThickSolid(myBody, faceToRemove, smallThickness);
  return myBody;
}

export function makePan(csg: OCC, _height: number, _radius: number) {
  const height = _height || 20;
  const radius = _radius || 25.0;
  const thickness = 1;
  const handleRadius = 4;
  const handleLength = 30;

  const s1 = csg.makeCylinder([0, 0, 0], [0, 0, height], radius);

  const s2 = csg.makeSphere([0, 0, 0], radius);

  const s3 = csg.makeCylinder(
    [0, 0, -radius * 0.7],
    [0, 0, height],
    radius * 2
  );
  const s4 = csg.fuse(s1, s2);
  const s5 = csg.common(s4, s3);

  const pt1: Triplet = [radius - 2 * thickness, 0, height - handleRadius * 1.1];
  const pt2: Triplet = [
    handleLength + radius - 2 * thickness,
    0,
    height - handleRadius * 1.1,
  ];
  const handle = csg.makeCylinder(pt1, pt2, handleRadius);

  const s6 = csg.fuse(s5, handle);

  const r1 = csg.makeCylinder([0, 0, 0], [0, 0, height], radius - thickness);
  const r2 = csg.makeSphere([0, 0, 0], radius - thickness);
  const r3 = csg.makeCylinder(
    [0, 0, -radius * 0.7 + thickness],
    [0, 0, height],
    radius * 2
  );
  const r4 = csg.fuse(r1, r2);
  const r5 = csg.common(r4, r3);

  let body = csg.cut(s6, r5);

  const lidHeight = 10;
  const lid = makePanLid(csg, radius, lidHeight, height);
  lid.translate([0, 0, 1]);

  body = csg.fuse(body, lid);

  return body;
}

export function makePanLid(csg: OCC, _r: number, _height: number, _H: number) {
  "use strict";

  // r : pan radius
  // height :
  const r = _r || 25.0;
  const h = _height || 10;
  const thickness = 1;

  // r**2 + (R-h)**2 = R**2
  // r**2 + R**2-2*h*R +h**2 = R**2
  // => R = ( r**2+h**2)/(2*h);

  const R = (r * r + h * h) / (2 * h);

  const center: Triplet = [0, 0, _H + h - R];

  const outerSphere = csg.makeSphere(center, R);
  const innerSphere = csg.makeSphere(center, R - thickness);
  let solid = csg.cut(outerSphere, innerSphere);

  const cyl = csg.makeCylinder([0, 0, _H + h - 3 * R], [0, 0, _H], R + r * 2);

  solid = csg.cut(solid, cyl);

  return solid;
}

export function makeRoundedPlate(
  csg: OCC,
  R1: number,
  R2: number,
  L: number,
  thickness: number
) {
  "use strict";

  R1 = R1 || 7;
  R2 = R2 || 2.5;

  L = L || 12;
  thickness = thickness || 1.5;

  const rad2deg = 180 / Math.atan(1.0) / 4.0;

  const sinAlpha = (R1 - R2) / L;
  const angle = Math.asin(sinAlpha) * rad2deg;
  const L2 = L * (1 - sinAlpha * sinAlpha);

  const q0: PointLike = [-200 * R1, 0, thickness];
  const p1: PointLike = [-R1, L2, 0];
  const p2: PointLike = [0, 0, thickness];
  const p3: PointLike = [R1, L2, 0];
  const q3: PointLike = [200 * R1, 0, thickness];

  let a = csg.makeBox(p1, p2);

  a = a.rotate(p2, [0, 0, 1], -angle);

  let b = csg.makeBox(p2, p3);
  b = b.rotate(p2, [0, 0, 1], angle);
  let v = csg.fuse(b, a);

  // remove unwanted material
  v = csg.cut(v, csg.makeBox(q0, p1).rotate(p2, [0, 0, 1], -angle));
  v = csg.cut(v, csg.makeBox(q3, p3).rotate(p2, [0, 0, 1], angle));

  //    return v;
  const c1 = csg.makeCylinder([0, 0, 0], [0, 0, thickness], R1);
  v = csg.fuse(v, c1);
  const c2 = csg.makeCylinder([0, L, 0], [0, L, thickness], R2);
  v = csg.fuse(v, c2);

  return v;
}

export function makeRivetPlate(
  csg: OCC,
  params: {
    B: number;
    C: number;
    H1: number;
    H2: number;
    J: number;
    K: number;
    R: number;
    V: number;
  }
) {
  // see http://www.tracepartsonline.net/%28S%281gpggj45ixmu5o5540hxuofo%29%29/PartsDefs/Production/ALCOA/22-02072002-063054/documents/AJAL103.pdf
  //  { A: 18,    B:7,    C:6.0,  F:4.7,  H:3.6,  H1:2.0,  H2:3.0,  J:6.0,  K:2.5,  R:2.5,  V:1.5 }
  //  { A: 24.3,  B:9.5,  C:8.5,  F:4.7,  H:5.3,  H1:2.8,  H2:4.8,  J:8.0,  K:2.5,  R:3.0,  V:1.5 }
  //  { A: 26.0,  B:11.0, C:9.5,  F:4.7,  H:6.0,  H1:3.5,  H2:5.5,  J:8.0,  K:2.5,  R:3.0,  V:1.5 }
  //  { A: 29.0,  B:13.0, C:11.0, F:4.7,  H:7.0,  H1:4.2,  H2:6.2,  J:8.0,  K:3.3,  R:3.5,  V:1.5 }
  //  { A: 33.5,  B:18.0, C:13.0, F:6.0,  H:9.3,  H1:5.1,  H2:8.5,  J:8.0,  K:3.3,  R:3.5,  V:1.5 }
  //xx  const A = params.A || 18;
  const B = params.B || 7;
  const C = params.C || 6;
  //xx  const F = params.F || 4.7;
  //xx  const H = params.H || 3.6;
  const H1 = params.H1 || 2.0;
  const H2 = params.H2 || 3.0;
  const J = params.J || 6.0;
  const K = params.K || 2.5;
  const R = params.R || 2.5;
  const V = params.V || 1.5;

  let base = exports.makeRoundedPlate(csg, B / 2, R, C + J, V);
  base = csg.fuse(base, csg.makeCylinder([0, 0, 0], [0, 0, H1], (B / 2) * 0.8));
  base = csg.fuse(
    base,
    csg.makeCone([0, 0, H1], (B / 2) * 0.8, [0, 0, H2], (B / 2) * 0.6)
  );
  base = csg.cut(base, csg.makeCylinder([0, 0, 0], [0, 0, H2], K / 2));
  base = csg.cut(base, csg.makeCylinder([0, C + J, 0], [0, C + J, H2], K / 2));
  base = csg.cut(base, csg.makeCylinder([0, C, 0], [0, C, V], K / 2));
  return base;
}

/**
 * params:
 *   r1 : radius of the cylinder and base of cone
 *   r2 : cone top face radius
 *   H  : cone height
 *   H2 : cylinder height
 *   rs : fillet radius or radius of the torus which is
 *         tangent to the cone and the cylinder.
 */
export function makeCylinderTorusCone(
  csg: OCC,
  r1: number,
  r2: number,
  H: number,
  H2: number,
  rs: number
) {
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
  const angle = Math.atan((r1 - r2) / H);
  // calculate the distance below the cylinder top
  // at which the torus is tangent to the cylinder
  const d = -Math.tan(angle / 2) * rs;
  // calculate the distance above the cone bottom
  // at which the torus is tangent to the cone
  const d2 = d + rs * Math.sin(angle);
  // calculate the radius of the cone at the
  // tangent edge with the torus.
  const r1b = r1 - rs * (1.0 - Math.cos(angle));

  const cyl = csg.makeCylinder([0, 0, -H2], [0, 0, d], r1);
  const cone = csg.makeCone([0, 0, d2], r1b, [0, 0, H], r2);
  const tore = csg.makeTorus([0, 0, d], [0, 0, 1], r1 - rs, rs);

  let v = csg.cut(tore, cone);
  v = csg.cut(v, cyl);

  v = csg.fuse(v, cone);
  v = csg.fuse(v, cyl);
  return v;
}

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
export function makeLidWithTorus(
  csg: OCC,
  r: number,
  h: number,
  rs: number,
  thickness: number
) {
  const r0 = r - rs;
  const h0 = h - rs;
  const tanAlpha = h0 / r0;
  const alpha = Math.atan(tanAlpha);

  const hyp0_2 = r0 * r0 + h0 * h0;
  // h0/hyp0 = (hyp0/2)/R0

  const R0 = hyp0_2 / 2.0 / h0;
  const R = R0 + rs;

  const center: Triplet = [0, 0, h0 - R0];

  const outerSphere = csg.makeSphere(center, R);
  const innerSphere = csg.makeSphere(center, R - thickness);
  let solid = csg.cut(outerSphere, innerSphere);

  // lets cut the sphere
  // lets cut the sphere
  const hh = R / 3;
  //        .
  //    .   | tan(a)=s/c => s=h*tan(
  //  +-----|-------
  const c1: Triplet = [center[0], center[1], center[2] + hh];
  const r1 = Math.tan(2 * alpha) * hh;
  const c2: Triplet = [center[0], center[1], center[2] + (R + hh)];
  const r2 = Math.tan(2 * alpha) * (R + hh);

  const cuttingcone = csg.makeCone(c1, r1, c2, r2);
  solid = csg.common(solid, cuttingcone);

  solid = csg.common(solid, cuttingcone);

  const cyl = csg.makeCylinder(center, [0, 0, 0], R);
  // lets add a torus
  let outerfillet = csg.makeTorus([0, 0, 0], [0, 0, 1], r0, rs);
  outerfillet = csg.cut(outerfillet, cuttingcone);
  outerfillet = csg.cut(outerfillet, cyl);

  let fillet = outerfillet;
  if (rs - thickness > 0) {
    let innerfillet = csg.makeTorus([0, 0, 0], [0, 0, 1], r0, rs - thickness);
    innerfillet = csg.cut(innerfillet, cuttingcone);
    fillet = csg.cut(fillet, innerfillet);
  }
  fillet = csg.cut(fillet, cuttingcone);
  return csg.fuse(solid, fillet);
  //xx return csg.compound([solid,fillet]);
}

export function makeTube(
  csg: OCC,
  p1: PointLike,
  p2: PointLike,
  R: number,
  thickness: number
) {
  const cyl1 = csg.makeCylinder(p1, p2, R);
  const cyl2 = csg.makeCylinder(p1, p2, R - thickness);
  return csg.cut(cyl1, cyl2);
}

export function makeHollowCylinder(
  csg: OCC,
  R: number,
  H: number,
  h: number,
  rf: number,
  thickness: number
) {
  let top = exports.makeLidWithTorus(csg, R, h, rf, thickness);
  let bottom = top.clone();
  bottom = bottom.rotate([0, 0, 0], [1, 0, 0], 180);

  const cyl = exports.makeTube(csg, [0, 0, 0], [0, 0, H], R, thickness);
  top = top.translate([0, 0, H]);
  let solid = csg.fuse(bottom, cyl);
  solid = csg.fuse(solid, top);
  return solid;
}

export function testHollowCylinder(csg: OCC) {
  const obj = exports.makeHollowCylinder(csg, 40, 100, 10, 5, 1);
  // create a section to verify visually the correctness of
  // the construction.
  const cuttingPlanes = csg.makeBox([0, 0, -100], [100, 200, 100]);
  return csg.cut(obj, cuttingPlanes);
}

export function makeLegoBrickSlow(
  csg: OCC,
  nX: UInteger,
  nY: UInteger,
  type: "thin" | "thick"
) {
  let h: number;

  if (type === "thin") {
    h = 2;
  } else if (type === "thick") {
    h = 6;
  } else {
    throw new Error("invalid");
  }

  const u = 1.6; // lego unit

  const outerWidth = nX * u * 5;
  const outerLength = nY * u * 5;
  const outerHeight = h * u;

  const outerBlock = csg.makeBox(
    [0, 0, 0],
    [outerWidth, outerLength, outerHeight]
  );

  const innerWidth = outerWidth - 2 * u;
  const innerLength = outerLength - 2 * u;
  const innerHeight = outerHeight - u;

  let innerBlock = csg.makeBox(
    [0, 0, 0],
    [innerWidth, innerLength, innerHeight]
  );
  innerBlock = innerBlock.translate([u, u, 0]);
  let hollowBlock = csg.cut(outerBlock, innerBlock);

  const pt1: Triplet = [2.5 * u, 2.5 * u, outerHeight - 3 * u];
  const pt2: Triplet = [2.5 * u, 2.5 * u, outerHeight + 3 * u];
  let h1 = csg.makeCylinder(pt1, pt2, 0.75 * u);

  const pt3: Triplet = [2.5 * u, 2.5 * u, outerHeight];
  const pt4: Triplet = [2.5 * u, 2.5 * u, outerHeight + u];
  let h2 = csg.makeCylinder(pt3, pt4, 1.5 * u);

  // installer la grille
  for (let y = 0; y < nY; y++) {
    let hh1 = h1.clone();
    let hh2 = h2.clone();
    for (let x = 0; x < nX; x++) {
      //
      hollowBlock = csg.cut(hollowBlock, hh1);
      hollowBlock = csg.fuse(hollowBlock, hh2);
      hh1 = hh1.translate([5 * u, 0, 0]);
      hh2 = hh2.translate([5 * u, 0, 0]);
    }
    h1 = h1.translate([0, 5 * u, 0]);
    h2 = h2.translate([0, 5 * u, 0]);
  }

  const pt5: Triplet = [2.5 * u, 2.5 * u, 0];
  const pt6: Triplet = [2.5 * u, 2.5 * u, outerHeight - u];
  let pinOuter = csg.makeCylinder(pt5, pt6, u);

  const pt7: Triplet = [2.5 * u, 2.5 * u, 0];
  const pt8: Triplet = [2.5 * u, 2.5 * u, outerHeight - u];
  let pinInner = csg.makeCylinder(pt7, pt8, 0.5 * u);

  let p = csg.cut(pinOuter, pinInner);
  let pp;
  if (nY == 1) {
    // install small pin insid
    p = p.translate([2.5 * u, 0, 0]);
    for (let x = 0; x < nX - 1; x++) {
      hollowBlock = csg.fuse(hollowBlock, p);
      p = p.translate([5 * u, 0, 0]);
    }
  }
  if (nX == 1) {
    p = p.translate([0, 2.5 * u, 0]);
    for (let y = 0; y < nY - 1; y++) {
      hollowBlock = csg.fuse(hollowBlock, p);
      p = p.translate([0, 5 * u, 0]);
    }
  }
  if (nX > 1 && nY > 1) {
    const pt9: Triplet = [5 * u, 5 * u, 0];
    const pt10: Triplet = [5 * u, 5 * u, outerHeight - u];
    pinOuter = csg.makeCylinder(pt9, pt10, (4.07 / 2.0) * u);

    const pt11: Triplet = [5 * u, 5 * u, 0];
    const pt12: Triplet = [5 * u, 5 * u, outerHeight - u];
    pinInner = csg.makeCylinder(pt11, pt12, 1.5 * u);

    let pin = csg.cut(pinOuter, pinInner);

    for (let x = 0; x < nX - 1; x++) {
      pp = pin.clone();
      for (let y = 0; y < nY - 1; y++) {
        hollowBlock = csg.fuse(hollowBlock, pp);
        pp = pp.translate([0, 5 * u, 0]);
      }
      pin = pin.translate([5 * u, 0, 0]);
    }
  }
  return hollowBlock;
}

function makeRepetition<T extends { clone(): T; translate(v: VectorLike): T }>(
  csg: OCC,
  shape: T,
  dX: number,
  nX: number,
  dY: number,
  nY: number
) {
  let h1 = shape.clone();
  // installer la grille
  const shapeArray: T[] = [];
  for (let y = 0; y < nY; y++) {
    let hh1 = h1.clone();
    for (let x = 0; x < nX; x++) {
      shapeArray.push(hh1);
      hh1 = hh1.translate([dX, 0, 0]);
    }
    h1 = h1.translate([0, dY, 0]);
  }
  return csg.compound(shapeArray);
}

export function makeLegoBrick(
  csg: OCC,
  nX: UInteger,
  nY: UInteger,
  type: "thin" | "thick"
) {
  let h = 0;
  if (type === "thin") {
    h = 2;
  } else if (type === "thick") {
    h = 6;
  } else {
    throw new Error("invalid h");
  }

  const u = 1.6; // lego unit

  const outerWidth = nX * u * 5;
  const outerLength = nY * u * 5;
  const outerHeight = h * u;

  let brick = csg.makeBox([0, 0, 0], [outerWidth, outerLength, outerHeight]);
  brick = csg.makeThickSolid(brick, brick.faces.bottom, -u);

  const pt1: Triplet = [2.5 * u, 2.5 * u, outerHeight];
  const pt2: Triplet = [2.5 * u, 2.5 * u, outerHeight + u];
  const h2 = csg.makeCylinder(pt1, pt2, 1.5 * u);

  let tetons = makeRepetition(csg, h2, 5 * u, nX, 5 * u, nY);
  brick = csg.fuse(brick, tetons);

  const pt3: Triplet = [2.5 * u, 2.5 * u, outerHeight - 3 * u];
  const pt4: Triplet = [2.5 * u, 2.5 * u, outerHeight + 0.75];
  const h1 = csg.makeCylinder(pt3, pt4, 0.74 * u);
  tetons = makeRepetition(csg, h1, 5 * u, nX, 5 * u, nY);
  brick = csg.cut(brick, tetons);

  //xx console.log(Object.keys(brick.faces));//.bottom);

  // small pins
  const pt5: Triplet = [2.5 * u, 2.5 * u, 0];
  const pt6: Triplet = [2.5 * u, 2.5 * u, outerHeight - u];
  let pinOuter = csg.makeCylinder(pt5, pt6, u);

  const pt7: Triplet = [2.5 * u, 2.5 * u, 0];
  const pt8: Triplet = [2.5 * u, 2.5 * u, outerHeight - u];
  let pinInner = csg.makeCylinder(pt7, pt8, 0.5 * u);

  let pin = csg.cut(pinOuter, pinInner);

  let p;
  if (nY == 1) {
    // install small pin insid
    p = pin.clone();
    p = p.translate([2.5 * u, 0, 0]);
    tetons = makeRepetition(csg, p, 5 * u, nX - 1, 0, 1);
    brick = csg.fuse(brick, tetons);
  } else if (nX == 1) {
    p = pin.clone();
    p = p.translate([0, 2.5 * u, 0]);
    tetons = makeRepetition(csg, p, 0, 1, 5 * u, nY - 1);
    brick = csg.fuse(brick, tetons);
  } else if (nX > 1 && nY > 1) {
    pinOuter = csg.makeCylinder(
      [5 * u, 5 * u, 0],
      [5 * u, 5 * u, outerHeight - u],
      (4.07 / 2.0) * u
    );
    pinInner = csg.makeCylinder(
      [5 * u, 5 * u, 0],
      [5 * u, 5 * u, outerHeight - u],
      1.5 * u
    );
    pin = csg.cut(pinOuter, pinInner);
    tetons = makeRepetition(csg, pin, 5 * u, nX - 1, 5 * u, nY - 1);
    brick = csg.fuse(brick, tetons);
  }

  return brick;
}

export function makePiston(occ: OCC) {
  // create the top wire
  //             ---|
  //           .    .
  //         /      |
  // 2+-----+       .
  //  |     3       |
  // 1+-------------.
  //  |<------------w--...>
  //

  const w = 65;
  const r = 20;
  const h = 12;

  const p0 = occ.makeVertex([0, 0, 0]);

  const p1 = p0.translate([-w / 2.0, 0, 0]);

  const p2 = p1.translate([0.0, h, 0]);
  const p3 = p0.translate([-r, h, 0]);
  const p4 = p0.translate([0, h + r, 0]);

  const trsf = occ.makePlaneMirror([0, 0, 0], [1, 0, 0]);

  const q1 = p1.transformed(trsf);
  const q2 = p2.transformed(trsf);
  const q3 = p3.transformed(trsf);

  const e1 = occ.makeLine(q1, p1);
  const e2 = occ.makeLine(p1, p2);
  const e3 = occ.makeLine(p2, p3);
  const e4 = occ.makeArc3P(p3, p4, q3);
  const e5 = occ.makeLine(q3, q2);
  const e6 = occ.makeLine(q2, q1);

  const wire1 = occ.makeWire(e1, e2, e3, e4, e5, e6);
  assert(6 === wire1.numEdges);

  const face1 = occ.makeFace(wire1);

  const height = 12;
  return occ.makePrism(face1, [0, 0, height]);
}

export function makeTutorialPart(occ: OCC) {
  const w = 60;
  const H = 50;
  const h = H / 2;
  const a = 7.5;
  const b = 20;

  const p0: Triplet = [b, -a / 2, 0];
  const p1: Triplet = [0, -a / 2, 0];
  const p2: Triplet = [0, -h, 0];
  const p3: Triplet = [w, -h, 0];
  const p4: Triplet = [w, h, 0];
  const p5: Triplet = [0, h, 0];
  const p6: Triplet = [0, a / 2, 0];
  const p7: Triplet = [b, a / 2, 0];

  const p8: Triplet = [b + a / 2, 0, 0];

  const e1 = occ.makeLine(p0, p1);
  const e2 = occ.makeLine(p1, p2);
  const e3 = occ.makeLine(p2, p3);
  const e4 = occ.makeLine(p3, p4);
  const e5 = occ.makeLine(p4, p5);
  const e6 = occ.makeLine(p5, p6);
  const e7 = occ.makeLine(p6, p7);
  const e8 = occ.makeArc3P(p7, p8, p0);

  const wire = occ.makeWire(e1, e2, e3, e4, e5, e6, e7, e8);
  assert(true === wire.isClosed);

  const height = 20;
  const face = occ.makeFace(wire);
  const body1 = occ.makePrism(face, [0, 0, height]);

  // --------------------------------------------------
  const height2 = 45;
  const circle = occ.makeCircle([w, 0, 0], [0, 0, 1], h);
  const wire2 = occ.makeWire(circle);
  const face2 = occ.makeFace(wire2);
  const body2 = occ.makePrism(face2, [0, 0, height2]);

  // -----------------------------------------------------
  const body3 = occ.fuse(body1, body2);

  //
  //     ------+
  //   /
  // +
  //    ------+
  //
  const R = 15;
  const angle = Math.asin(7.5 / R);
  const dx = R * Math.cos(angle);
  const dy = R * Math.sin(angle);

  const q1: Triplet = [w + dx, dy, 0];
  const q2: Triplet = [w + dx + 100, dy, 0];
  const q3: Triplet = [w + dx + 100, -dy, 0];
  const q4: Triplet = [w + dx, -dy, 0];
  const q5: Triplet = [w - R, 0, 0];

  const ee1 = occ.makeLine(q1, q2);
  const ee2 = occ.makeLine(q2, q3);
  const ee3 = occ.makeLine(q3, q4);
  const ee4 = occ.makeArc3P(q4, q5, q1);
  const wire4 = occ.makeWire(ee1, ee2, ee3, ee4);
  const face4 = occ.makeFace(wire4);
  const body4 = occ.makePrism(face4, [0, 0, height2]);

  const body5 = occ.cut(body3, body4);
  const edges = body5.getEdges();
  // --------------------------------------------
  // Select vertical edges with vertex P1 and P6
  // --------------------------------------------
  function same(a: number, b: number, tol: number) {
    return Math.abs(a - b) < tol;
  }

  function samePoint(p1: IPoint, p2: IPoint) {
    const tol = 0.001;
    return (
      same(p1.x, p2.x, tol) && same(p1.y, p2.y, tol) && same(p1.z, p2.z, tol)
    );
  }

  function selectEdge(edges: IEdge[], p: PointLike) {
    if (p instanceof Array) {
      p = occ.makeVertex(p);
    }
    const results = edges.filter((edge) => {
      const firstVertex = edge.firstVertex;
      const lastVertex = edge.lastVertex;
      return (
        (samePoint(firstVertex, p as IPoint) ||
          samePoint(lastVertex, p as IPoint)) &&
        same(firstVertex.x, lastVertex.x, 0.01) &&
        same(firstVertex.y, lastVertex.y, 0.01)
      );
    });
    return results[0];
  }

  const edges_for_filet = [selectEdge(edges, p2), selectEdge(edges, p5)];

  const body6 = occ.makeFillet(body5, edges_for_filet, 10);

  // create hole
  const smallR = 5;
  const heigth3 = height2 - smallR - 10;
  const cyl = occ.makeCylinder(
    [w - R - 10, 0, heigth3],
    [w - R + 20, 0, heigth3],
    smallR
  );

  return occ.cut(body6, cyl);
}
