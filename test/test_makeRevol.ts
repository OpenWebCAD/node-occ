import { occ, IVertex, IEdge, IAxisLike } from "..";
import { dumpSolid } from "./debug_tools";
const { makeVertex, makeRevol, makeLine, makeWire, makeFace } = occ;

describe("test makeRevol", function () {
  it("should revolve a vertex into a egde", () => {
    const zAxis: IAxisLike = [
      [0, 0, 0],
      [0, 0, 1]
    ];

    const vertex: IVertex = makeVertex([10, 0, 0]);
    const edge = makeRevol(vertex, zAxis, 180);
  });

  it("should revolve a edge into a wire", () => {
    const zAxis: IAxisLike = [
      [0, 0, 0],
      [0, 0, 1]
    ];

    const edge = makeLine([10, 0, 0], [10, 0, 10]);
    const wire = makeRevol(edge, zAxis, 30);
  });

  it("should revolve a wire into a face", () => {
    const zAxis: IAxisLike = [
      [0, 0, 0],
      [0, 0, 1]
    ];
    const edge1 = makeLine([10, 0, 0], [10, 0, 10]);
    const edge2 = makeLine([10, 0, 10], [5, 0, 20]);
    const wire = makeWire([edge1, edge2]);

    const face = makeRevol(wire, zAxis, 180);
  });

  it("should revolve a face into a solid", () => {
    const zAxis: IAxisLike = [
      [0, 0, 0],
      [0, 0, 1]
    ];

    const circle = occ.makeCircle([100, 0, 0], [1, 0, 0], 10);
    const wire = makeWire(circle);
    const face = makeFace(wire);
    const solid = makeRevol(face, zAxis, 180);
    dumpSolid(solid);
  });
});
