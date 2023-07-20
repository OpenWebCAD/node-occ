import { IShell, ISolid } from "..";

const doDebug = true;
export function dumpSolid(b: ISolid) {
  if (doDebug) {
    console.log("num faces    ", b.numFaces);
    console.log("num shells   ", b.numShells);
    console.log("num edges    ", b.getEdges().length);
    console.log("num vertices ", b.getVertices().length);
    console.log(
      " faces    = ",
      b
        .getFaces()
        .map((e) => b.getShapeName(e))
        .join(", ")
    );
    console.log(
      " edges    = ",
      b
        .getEdges()
        .map((e) => b.getShapeName(e))
        .join(", ")
    );
    console.log(
      " vertices = ",
      b
        .getVertices()
        .map((e) => b.getShapeName(e))
        .join(", ")
    );
  }
}
export function dumpShell(b: IShell) {
  if (doDebug) {
    console.log("num faces    ", b.numFaces);
    console.log("num edges    ", b.getEdges().length);
    console.log("num vertices ", b.getVertices().length);
    console.log(
      " faces    = ",
      b
        .getFaces()
        .map((e) => b.getShapeName(e))
        .join(", ")
    );
    console.log(
      " edges    = ",
      b
        .getEdges()
        .map((e) => b.getShapeName(e))
        .join(", ")
    );
    console.log(
      " vertices = ",
      b
        .getVertices()
        .map((e) => b.getShapeName(e))
        .join(", ")
    );
  }
}
