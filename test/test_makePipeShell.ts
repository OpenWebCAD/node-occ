import should from "should";
import { Triplet, IWire,Vertex, occ } from "../dist";
import { dumpSolid, dumpShell } from "./debug_tools";
import { getTemporaryFilePath } from "./helpers";


const rotate = (point: Triplet, angleInDegree: number): Triplet => {

  // x2=cosβ*x−sinβ*y
  // y2 = sinβ*x + cosβ*y
  const angleInRadian = angleInDegree * Math.atan(1) / 45.0;

  const cosβ = Math.cos(angleInRadian);
  const sinβ = Math.sin(angleInRadian);
  const r = (x: number, y: number) => [cosβ * x - sinβ * y, sinβ * x + cosβ * y, 0];
  const [x, y, z] = point;
  const [xp, yp] = r(x, y);
  return [xp, yp, z];
}
const rotTrans = (p1: Triplet, angleInDegree: number, deltaZ: number): Triplet => {
  const [x, y, z] = rotate(p1, angleInDegree);
  return [x, y, z + deltaZ]
}
interface IMakeHelixParams {
  startPoint: Triplet;
  angleInDegree: number, height: number, nbPoints: number;
}
function makeHelixPoints({ startPoint, angleInDegree, height, nbPoints }: IMakeHelixParams) {
  const points: Triplet[] = [startPoint];
  for (let index = 1; index <= nbPoints; index++) {
    const p = rotTrans(startPoint, angleInDegree / nbPoints * index, height / nbPoints * index)
    points.push(p);
  }
  return points
}
describe("makePipeShell", () => {
  it("should make a pipe from a spine which is a  wire (circle) and a profile line ", () => {

    const wire = occ.makeWire(occ.makeCircle([0, 0, 0], [0, 0, 1], 10));

    const spine = occ.makeWire(occ.makeInterpolatedCurve(
      [
        [0, 0, 0],
        [0, 6, 50],
        [0, 0, 100]
      ],
      false,
      0.01
    ));

    const shell = occ.makePipeShell({
      wire,
      spineSupport: spine,
      solid: false,
    });

    dumpShell(shell);

    const b1_step = "pipe.step"; // getTemporaryFilePath({ prefix: "pipe_", suffix: ".step" });
    occ.writeSTEP(b1_step, shell as any);

  });


  it("should make a pipe from a spine which is a  wire (circle) and a profile line ", () => {


    // the triangle shape we want to twist along the z axis
    const p1: Triplet = [10, 0, 0];
    const p2: Triplet = [8, 2, 0];
    const p3: Triplet = [8, -2, 0];
    const edge1 = occ.makeLine(p1, p2);
    const edge2 = occ.makeLine(p2, p3);
    const edge3 = occ.makeLine(p3, p1);
    const wire = occ.makeWire([edge1, edge2, edge3]);

    // the spine is the Z -axis
    const spine = occ.makeWire(occ.makeLine([0, 0, 0], [0, 0, 10]));


    const angleInDegree = 180;
    const height = 40;
    const points = makeHelixPoints({ startPoint: p1, angleInDegree, height, nbPoints: 10 });

    // we want a auxilary curve, one point should coincide with one edge of the triangle and 
    const auxilaryCurve = occ.makeWire(occ.makeInterpolatedCurve(points, false, 0.01));

    const wires: IWire[] = [
      wire,
      // wire.rotate([0, 0, 0], [0, 0, 1], angleInDegree/4).translate([0, 0, height/4]),
      // wire.rotate([0, 0, 0], [0, 0, 1], angleInDegree/4*2).translate([0, 0, height/2]),
      // wire.rotate([0, 0, 0], [0, 0, 1], angleInDegree/4*3).translate([0, 0, height/4*3]),
      wire.rotate([0, 0, 0], [0, 0, 1], angleInDegree).translate([0, 0, height])
    ];
    // we are defining a shape
    const solid = occ.makePipeShell({
      wires,
      spineSupport: spine,
      auxilaryCurve,
      solid: false,
    });




    const b2_step = "pipe2.step";
    occ.writeSTEP(b2_step, solid);

  });

  // it("should make a pipe from a spine which is a  Face and a profile line ", () => {

  //   const wire1 = occ.makeWire(occ.makeCircle([0, 0, 0], [0, 0, 1], 10));
  //   const face = occ.makeFace(wire1);

  //   const spine = occ.makeWire(occ.makeInterpolatedCurve(
  //     [
  //       [0, 0, 0],
  //       [0, 6, 10],
  //       [0, 0, 20],

  //     ], false, 0.01));

  //   const solid = occ.makePipe(face, spine);
  //   dumpSolid(solid);
  // });
});
