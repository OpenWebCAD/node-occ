
import { occ } from "..";
import { getTemporaryFilePath } from "./helpers";
import { makeBottle } from "..";

describe("testing geometry builder", function () {
    it("should create a bottle", function () {

        let bottle_brep = getTemporaryFilePath({ prefix: "bottle", suffix: ".brep" });
        let bottle = makeBottle(occ, { height: 100, filletRadius: 2 });
        occ.writeBREP(bottle_brep, bottle);
    });
});

