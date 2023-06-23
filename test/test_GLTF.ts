import { occ } from "..";

import { removeFile, getTemporaryFilePath } from "./helpers";

describe("testing GTLF", () => {
  it("should output a GLTF file", () => {
    const outputFile = getTemporaryFilePath({ prefix: "b1_", suffix: ".gtlf" });
    let box = occ.makeBox([0, 0, 0], [100, 200, 300]);
    let b1_result = occ.writeGLTF(outputFile, box);

    console.log("result", outputFile);
  });
});
