
export type Real = number;
export type UInteger = number;


export interface IPoint {
    x: Real,
    y: Real,
    z: Real,

    equals(otherPoint: PointLike): boolean;
    asArray(): Triplet;
}

export interface IVector {
    u: Real,
    v: Real,
    w: Real
}

export interface XYZ { x: number, y: number, z: number }

export type Triplet = [Real, Real, Real];
export type PointLike = IPoint | Triplet | XYZ;
export type VectorLike = IVector | Triplet;

export interface IBoundingBox {
    nearPt: IPoint;
    farPt: IPoint;

    /**
     * isVoid is true if the box is degenerated (i.e does not enclose any point)
     */
    isVoid: boolean;

    addPoint(point: PointLike): void;
    isOut(point: PointLike): boolean;


    /**
     * return a array containing the 8 corners of the bounding box
     */
    getVertices(): IPoint[];
}

export interface IMesh {

    numVertices: UInteger;
    numEdges: UInteger;
    numTriangles: UInteger;

    vertices: Triplet[];

    normals: Float32Array;

    edgeIndices: UInteger[];
    triangles: [UInteger, UInteger, UInteger][];
    triangleNormals: UInteger[];

    faceRanges: UInteger[];
    edgeRanges: UInteger[];


    getFaceTriangles(face: IFace): Uint8Array | Uint16Array | Uint32Array;
    getEdgeIndices(edge: IEdge): Uint8Array | Uint16Array | Uint32Array;
    getFaceTriangleNormals(face: IFace): Uint8Array | Uint16Array | Uint32Array;


    toJSON(): string;
}

export interface ITransform {
    scaleFactor: Real;
    makeTranslation(point: PointLike): void;
    makeTranslation(x: Real, y: Real, z: Real): void;
    makePlaneMirror(point: PointLike, vector: VectorLike): ITransform;

}

export interface IShape<T> {

    shapeType: "SHELL" | "SOLID" | "COMPOUND" | "EDGE" | "FACE";
    name?: string;

    translate(vector: VectorLike): T;
    translate(dx: Real, dy: Real, dz: Real): T;

    rotate(rotationCenter: PointLike, vector: VectorLike, angleInDegree: Real): T;
    applyTransform(transformation: ITransform): T;
    scale(): T;
    mirror(): T;
    fixShape(): T;
    clone(): T;
    getBoundingBox(): IBoundingBox;
    transformed(transformation: ITransform): T;
}

export interface IVertex extends IShape<IVertex>, IPoint {
    new(x: Real, y: Real, z: Real): IVertex;
    new(point: PointLike): IVertex;
    isValid: boolean;
}

export interface IEdge extends IShape<IEdge> {

    /**
     * true if the edge is collapsed or degenerated
     */
    isNull: boolean;
    /**
     * true if the edge is degenerated
     */
    isDegenerated: boolean;

    /**
     * isClosed
     */
    isClosed: boolean;
    /**
     * isSeam
     */
    isSeam: boolean;

    /**
     * length the curvature lengh of the edge segment
     */
    length: Real;

    /**
     * numVertices number of vertices that this edge has.
     */
    numVertices: UInteger;


    firstVertex: IVertex;
    lastVertex: IVertex;

    /**
     * compute and return the bounding box of the edge.
     * 
     */
    getBoundingBox(): IBoundingBox;



    /**
     * 
     */
    polygonize(): Float32Array;

    getVertices(): IVertex[];
}

export interface IFace extends IShape<IFace> {
    numWires: UInteger;
    isPlanar: boolean;
    centreOfMass: IPoint;
    /**
     * the area of the face  in mm^2
     */
    area: Real;

    getWires(): IWire[];

    hasMesh: boolean;

    mesh: IMesh;
    createMesh(tol: Real, angle?: Real): IMesh;

    getBoundingBox(): IBoundingBox;


}

export interface IShell extends IShape<IShell> {
    shapeType: "SHELL";

    hashCode: string;

    numFaces: UInteger;
    orientation: "FORWARD";


}

export interface IShapeIterator<T> {
    // constructor(solid: ISolid, type: "FACE" | "EDGE");
    more: boolean;
    current: T;
    next(): T;
}


export interface ISolid extends IShape<ISolid> {

    shapeType: "SOLID" | "COMPOUND";

    numFaces: UInteger;
    numSolids: UInteger;
    numShells: UInteger;
    outerShell: IShell;


    volume: Real;
    area: Real;


    getEdges(): IEdge[];
    getFaces(): IFace[];
    getVertices(): IVertex[];
    getShells(): IShell[];

    /**
     * return a list of individual ISolid(SOLID) if self is a COMPOUND solid        
     */
    getSolids(): ISolid[];


    getOuterShell(): IShell;


    getCommonEdges(face1: IFace, face2: IFace): IEdge[];
    getAdjacentFaces(face: IFace): IFace[];


    // accessed to named faces
    faces: {
        [key: string]: IFace;
    }

    hasMesh: boolean;
    mesh: IMesh;
    createMesh(tolerance: Real, angle?: Real): IMesh;


    getShapeName(topologyElement: IFace | IEdge | IVertex): string;
}

export interface IWire {
    isClosed: boolean;
    numEdges: number;
    numVertices: number;
    length: number;
    isDegenerated: boolean;

    transformed(transform: ITransform): IWire;
    translate(vector: VectorLike): IWire;

    getEdges(): IEdge[];
    getVertices(): IVertex[];
    getBoundingBox(): IBoundingBox;
    polygonize(): Float32Array;
}

export interface OCC {

    makeBox(point1: PointLike, point2: PointLike): ISolid;
    makeBox(width: Real, length: Real, height: Real): ISolid;

    makeCylinder(lowCenterPoint: PointLike, highCenterPoint: PointLike, radius: Real): ISolid;
    makeCylinder([origin, axis]: [PointLike, VectorLike], radius: Real, height: Real): ISolid;
    makeCylinder(radius: Real, height: Real): ISolid;
    makeCylinder(originDirection: [PointLike, VectorLike], radius: Real, height: Real): ISolid;

    makeSphere(center: PointLike, radius: Real): ISolid;
    makeCone(p1: PointLike, r1: Real, p2: PointLike, r2: Real): ISolid;
    makeCone(radius1: Real, radius2: Real, height: Real): ISolid;
    makeCone(p1: PointLike, p2: PointLike, angle: Real, height: Real): ISolid;
    //
    makePrism(face: IFace, vector: VectorLike): ISolid;
    makeFillet(body: ISolid, edges: IEdge[] | IEdge, radius: number): ISolid;
    makeTorus(center: PointLike, vector: VectorLike, mainRadius: Real, smallRadius: Real): ISolid;

    // boolean operation
    // Wrires

    makeLine(point1: PointLike, point2: PointLike): IWire;
    makeArc3P(point1: PointLike, point2: PointLike, point3: PointLike): IWire;
    makeCircle(center: PointLike, normal: VectorLike, radius: Real): IWire;
    makeWire(...args: IWire[]): IWire;


    // --- TO IMPLEMENT SOMEHOW !!!
    makeVertex(): IVertex;
    makeVertex(point: PointLike): IVertex;
    makeVertex(x: Real, y: Real, z: Real): IVertex;
    // --- END TO IMPLEMENT SOMEHOW !!!

    makeFace(wire: IWire): IFace;

    // Boolean operations:

    fuse(solid1: ISolid, solid2: ISolid): ISolid;
    cut(solid1: ISolid, solid2: ISolid): ISolid;
    common(solid1: ISolid, solid2: ISolid): ISolid;

    compound<T>(shapes: T[]): T;
    compound(shapes: ISolid[]): ISolid;

    makeThickSolid(body: ISolid, faceToRemove: IFace, thickness: number): ISolid;
    makeDraftAngle(body: ISolid, face: IFace, angleInRadian: number, otherFace: IFace): ISolid;

    // 
    buildSolidMesh(solid: ISolid): any;
    buildSolidMeshNew(solid: ISolid): any;


    makePlaneMirror(point: PointLike, vector: VectorLike): ITransform;

    gc(): void;


    writeSTEP(filename: string, solid: ISolid): boolean;
    writeSTEP(filename: string, solids: ISolid[]): boolean;
    writeSTEP(filename: string, ...solids: ISolid[]): boolean;
    readSTEP(filename: string, callback: (err: Error | null, solids: ISolid[]) => void): void;

    writeBREP(filename: string, solid: ISolid): boolean;
    writeBREP(filename: string, solids: ISolid[]): boolean;
    writeBREP(filename: string, ...solids: ISolid[]): boolean;

    readBREP(filename: string, callback: (err: Error | null, solids: ISolid[]) => void): void;
}