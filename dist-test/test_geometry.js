"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const __1 = require("..");
const helpers_1 = require("./helpers");
const __2 = require("..");
describe("testing geometry builder", function () {
    it("should create a bottle", function () {
        let bottle_brep = (0, helpers_1.getTemporaryFilePath)({ prefix: "bottle", suffix: ".brep" });
        let bottle = (0, __2.makeBottle)(__1.occ, { height: 100, filletRadius: 2 });
        __1.occ.writeBREP(bottle_brep, bottle);
    });
});
