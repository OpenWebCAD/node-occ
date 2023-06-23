import should from "should";
import { Vertex, occ } from "..";
describe("makePipe", () => {
  it("should make a pipe from 2 circles", () => {
    const wire1 = occ.makeCircle([0, 0, 0], [0, 0, 1], 10);

    const spine = occ.makeLine([0, 0, 0], [0, 0, 10]);

    ///   const solid = occ.makePipe(spine, wire1);
  });
});
