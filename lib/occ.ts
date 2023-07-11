/* global exports,require */
/**
 * Module dependencies.
 */
import { assert } from "console";
import {
  Real,
  IEdge,
  IFace,
  ISolid,
  Triplet,
  IPoint,
  IVertex,
  PointLike,
  IBoundingBox,
  ITransform,
  IVector,
  IWire,
  OCC,
  IShapeIterator
} from "./interfaces";

export const _occ = require("../lib/bindings");
export const occ = _occ as OCC;
exports.version = "0.12";
exports.description = "OpenCascade Wrapper for NodeJS";

// ----------------------------------------------------------------------------- BoundingBox
export declare interface BoundingBox1 extends IBoundingBox {}
export declare class BoundingBox1 implements IBoundingBox {
  constructor();
  constructor(point: PointLike);
  constructor(point1: PointLike, point2: PointLike);
}
export const BoundingBox = _occ.BoundingBox as typeof BoundingBox1;

BoundingBox.prototype.toString = function () {
  return (
    "{ nearPt: " +
    this.nearPt.toString() +
    " , farPt: " +
    this.farPt.toString() +
    "}"
  );
};
BoundingBox.prototype.toString = function () {
  return (
    "[ " +
    this.nearPt.x.toFixed(3) +
    "," +
    this.nearPt.y.toFixed(3) +
    "," +
    this.nearPt.z.toFixed(3) +
    "]" +
    "[ " +
    this.farPt.x.toFixed(3) +
    "," +
    this.farPt.y.toFixed(3) +
    "," +
    this.farPt.z.toFixed(3) +
    "]"
  );
};

// ----------------------------------------------------------------------------- Point
export interface Point1 extends IPoint {}
export declare class Point1 implements IPoint {
  constructor(x: Real, y: Real, z: Real);
}
export const Point = _occ.Point3D as typeof Point1;
Point.prototype.toString = function () {
  return (
    "{" +
    " x: " +
    f(this.x, 8, 3) +
    "," +
    " y: " +
    f(this.y, 8, 3) +
    "," +
    " z: " +
    f(this.z, 8, 3) +
    "" +
    " }"
  );
};

// ----------------------------------------------------------------------------- Vector
export interface Vector1 extends IVector {}
export declare class Vector1 implements IVector {
  constructor(u: Real, v: Real, w: Real);
}
export const Vector = _occ.Vector3D as typeof Vector1;
// add utilities
export const XDir: Triplet = [1, 0, 0];
export const YDir: Triplet = [0, 1, 0];
export const ZDir: Triplet = [0, 0, 1];

// ----------------------------------------------------------------------------- Transformation

export interface Transformation1 extends ITransform {}
export declare class Transformation1 implements ITransform {}
export const Transformation = _occ.Transformation as typeof Transformation1;

// ----------------------------------------------------------------------------- Vertex
export interface Vertex1 extends IVertex {}
export declare class Vertex1 implements IVertex {}
export const Vertex = _occ.Vertex as Vertex1;
(Vertex as any).prototype.toString = Point.prototype.toString;

// ----------------------------------------------------------------------------- Edge
export interface Edge1 extends IEdge {}
export declare class Edge1 implements IEdge {}
export const Edge = _occ.Edge as typeof Edge1;
function f(a: number, w: number, p: number) {
  return ("          " + (a + 0).toFixed(p)).substr(-w);
}
(Edge as any).prototype.toString = function (this: IEdge) {
  return (
    "Edge => { \n" +
    "  numVertices: " +
    this.numVertices +
    "\n" +
    "  vertices: " +
    this.getVertices()
      .map((f) => {
        return f.toString();
      })
      .join(" ") +
    "}"
  );
};

export const _ShapeIterator = _occ.ShapeIterator;

export interface EdgeIterator extends IShapeIterator<IEdge> {}
export class EdgeIterator
  extends _ShapeIterator
  implements IShapeIterator<IEdge>
{
  constructor(faceOrSolid: IFace | ISolid) {
    super(faceOrSolid, "EDGE");
  }
}

// ----------------------------------------------------------------------------- Wire
export interface Wire1 extends IWire {}

export declare class Wire1 {
  constructor();
}
export const Wire = _occ.Wire as typeof Wire1;

// ----------------------------------------------------------------------------- Face
export interface Face1 extends IFace {}
export declare class Face1 implements IFace {}
export const Face = _occ.Face as typeof Face1;
assert(Face);

export interface FaceIterator extends IShapeIterator<IFace> {}
export class FaceIterator
  extends _ShapeIterator
  implements IShapeIterator<IFace>
{
  constructor(solid: ISolid) {
    super(solid, "FACE");
  }
}

// ----------------------------------------------------------------------------- Solid
export interface Solid1 extends ISolid {}
export declare class Solid1 {}
export const Solid = _occ.Solid as typeof Solid1;
assert(Solid);

// ----------------------------------------------------------------------------- Mesh

export const makeLine = _occ.makeLine;
export const makeArc3P = _occ.makeArc3P;
export const makeVertex = _occ.makeVertex;
export const makeWire = _occ.makeWire;
export const makeBox = _occ.makeBox;
export const makeCylinder = _occ.makeCylinder;

// complete object prototypes with pure Javascript
import { init as initMesh } from "./mesh";
import { init as initShape } from "./shape";
initMesh();
initShape(_occ);
