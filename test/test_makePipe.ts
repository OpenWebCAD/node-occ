import should from "should";
import { Vertex, occ } from "..";
import { dumpSolid, dumpShell } from "./debug_tools";
describe("makePipe", () => {
  it("should make a pipe from a spine which is a  wire (circle) and a profile line ", () => {
    const wire1 = occ.makeWire(occ.makeCircle([0, 0, 0], [0, 0, 1], 10));

    const spine = occ.makeInterpolatedCurve(
      [
        [0, 0, 0],
        [0, 6, 10],
        [0, 0, 20]
      ],
      false,
      0.01
    );
    const wire = occ.makeWire(spine);
    const shell = occ.makePipe(wire, wire1);
    dumpShell(shell);
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
