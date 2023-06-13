import assert from "assert";
import fs from "fs";
import { OCC, ISolid, Triplet } from "./interfaces";
import { occ } from "./occ";

function debugLog(...args: any[]) {
  arguments;
}

function export_debug_step(solid: ISolid) {
  let debug_step_file = "debug_step";
  let counter = 0;
  while (fs.existsSync(debug_step_file + counter.toString() + ".step")) {
    counter++;
  }
  debug_step_file = debug_step_file + counter.toString() + ".step";

  debugLog("The solid causing problem has been saved in ", debug_step_file);
  occ.writeSTEP(debug_step_file, solid);
}

export type TypedArray =
  | Uint8Array
  | Uint16Array
  | Uint32Array
  | Float32Array
  | Float64Array;

type Base64EncodingBuffer = number[];

interface IFaceMesh {
  name: string;
  indexes?: Base64EncodingBuffer;
  normalIndexes?: Base64EncodingBuffer;
  mesh?: any;
}

export interface IMesh2 {
  name?: string;
  edges: any;
  version: string;
  vertices: any;
  faces: IFaceMesh[];
  normals: any;
}

export function init(occ: OCC) {
  // Solid.prototype.getAllEdges = function () {
  //     // deprecated : use Solid.getEdgesi !
  //     const edges: IEdge[] = [];
  //     const iterator = new ShapeIterator<IEdge>(this, "EDGE");
  //     while (iterator.more) {
  //         edges.push(iterator.next());
  //     }
  //     return edges;
  // };
  // Face.prototype.getAllEdges = Solid.prototype.getAllEdges;
  // Wire.prototype.getAllEdges = Solid.prototype.getAllEdges;

  occ.buildSolidMeshNew = (solid: ISolid) => {
    function makeEdgesIndexes(solid: ISolid, jsonSolidMesh: IMesh2) {
      // produce each edge
      const mesh = solid.mesh;
      let indexes: Uint8Array | Uint16Array | Uint32Array | undefined =
        undefined;

      solid.getEdges().forEach((edge) => {
        if (edge.isSeam) {
          return;
        }
        try {
          indexes = mesh.getEdgeIndices(edge);
        } catch (err) {
          // fall back to complete edge
          const polyline = edge.polygonize();
          if (!polyline) {
            debugLog(
              "mesh# failed ! cannot extract edges indexes on solid",
              err,
              solid.hasMesh,
              solid.name,
              solid.getShapeName(edge)
            );
            debugLog(" mesh", mesh.numVertices);
            debugLog(" edge", edge.polygonize());
            export_debug_step(solid);
            //xx process.exit(2);
          }

          const entry = {
            name: solid.getShapeName(edge),
            mesh: toBase64(polyline),
          };
          jsonSolidMesh.edges.push(entry);
          return;
        }
        if (!indexes || indexes.length == 0) {
          return; // ignore empty edges
        }

        const entry = {
          name: solid.getShapeName(edge),
          indexes: toBase64(indexes),
        };
        jsonSolidMesh.edges.push(entry);
      });
    }

    Uint16Array;
    function toBase64_1(typedArray: TypedArray) {
      return Buffer.from(typedArray.buffer).toString("base64");
    }

    toBase64_1;

    function toBase64(
      typedArray: TypedArray | number[] | Triplet[]
    ): Base64EncodingBuffer {
      const a = [];
      for (let i = 0; i < typedArray.length; i++) {
        a[i] = typedArray[i];
      }
      return a as any /* FIX ME */;
    }

    function makeFacesIndexes(solid: ISolid, jsonSolidMes: IMesh2) {
      const mesh = solid.mesh;
      solid.getFaces().forEach((face) => {
        const indexes = mesh.getFaceTriangles(face);
        const normalindexes = mesh.getFaceTriangleNormals(face);

        const entry = {
          name: solid.getShapeName(face),
          indexes: toBase64(indexes),
          normalIndexes: toBase64(normalindexes),
        };
        //xx entry.mesh.materials[0].colorDiffuse = [ Math.random(),Math.random(),Math.random()];
        jsonSolidMesh.faces.push(entry);
      });
    }

    assert(
      solid.hasOwnProperty("name"),
      "occ.buildSolidMesh : the solid must have a name"
    );

    // make sure object is meshed
    if (!solid.hasMesh) {
      try {
        solid.createMesh(0.5, 5);
      } catch (err) {
        //
      }
      assert(solid.hasMesh);
    }
    const mesh = solid.mesh;

    const jsonSolidMesh: IMesh2 = {
      name: solid.name,
      version: "",
      vertices: [],
      faces: [],
      edges: [],
      normals: [],
    };

    jsonSolidMesh.vertices = toBase64(mesh.vertices);
    jsonSolidMesh.normals = toBase64(mesh.normals);

    makeFacesIndexes(solid, jsonSolidMesh);

    makeEdgesIndexes(solid, jsonSolidMesh);

    jsonSolidMesh.version = "2.0";
    return jsonSolidMesh;
  };

  occ.buildSolidMesh = function (solid: ISolid) {
    assert(
      solid.hasOwnProperty("name"),
      "occ.buildSolidMesh : the solid must have a name"
    );

    // make sure object is meshed
    const mesh = solid.mesh;
    mesh;

    // produce each faces
    const faces = solid.getFaces();
    let face;

    const jsonSolidMesh: IMesh2 = {
      version: "",
      name: solid.name,
      vertices: [],
      faces: [],
      edges: [],
      normals: [],
    };
    let i;

    for (i = 0; i < faces.length; i++) {
      face = faces[i];

      if (!face.hasMesh) {
        continue;
      }
      const entry: IFaceMesh = {
        name: solid.getShapeName(face),
        // color:(r*255+g)*255+b,
        mesh: face.mesh.toJSON() as any,
      };
      entry.mesh.materials[0].colorDiffuse = [
        Math.random(),
        Math.random(),
        Math.random(),
      ];
      jsonSolidMesh.faces.push(entry);
    }
    // produce each edge

    const edges = solid.getEdges();
    let edge;
    for (i = 0; i < edges.length; i++) {
      edge = edges[i];

      if (edge.isSeam) {
        continue;
      }
      const polygone = edge.polygonize();

      //xx console.log( "polygone.length = ",polygone.length);
      const pts = [];
      for (let j = 0; j < polygone.length; j++) {
        pts.push(polygone[j]);
      }
      const entry: IFaceMesh = {
        name: solid.getShapeName(edge),
        mesh: pts,
      };
      //entry.mesh.materials[0].colorDiffuse could be [ Math.random(),Math.random(),Math.random()];
      jsonSolidMesh.edges.push(entry);
    }
    jsonSolidMesh.version = "1.0";
    return jsonSolidMesh;
  };
}
