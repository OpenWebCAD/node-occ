
const occ = require("../lib/occ");
const shapeFactory = require("../lib/shapeFactory");


const getTemporaryFilePath = require("gettemporaryfilepath");


describe("testing geometry builder",function(){
    it("should create a bottle", function () {

        let bottle_brep = getTemporaryFilePath({prefix: "bottle", suffix: ".brep"});
        let bottle = shapeFactory.makeBottle(occ);
        occ.writeBREP(bottle_brep, bottle);
    });
});

