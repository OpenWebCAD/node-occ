
import { IBoundingBox, ISolid, Transformation, Triplet, occ } from "..";
import "should";

describe("testing various transformation", function () {


    function getVerticeData(box: IBoundingBox | ISolid): Triplet[] {
        let vert = box.getVertices();
        const triplets = vert.map((v) => [v.x, v.y, v.z] as Triplet);
        triplets.length.should.eql(8);
        return triplets;
    }
    function add(v1: Triplet, v2: Triplet): Triplet {
        return [v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]];
    }

    it("#applyTransform", function () {

        let box = occ.makeBox([0, 0, 0], [100, 200, 300]);

        let trsf = new Transformation();
        trsf.makeTranslation([10, 20, 30]);

        let vert = getVerticeData(box);

        vert[1].should.eql([0, 0, 0]);
        box.applyTransform(trsf);

        let vert_after = getVerticeData(box);

        // translate vertex
        vert = vert.map((v) => add(v, [10, 20, 30]));
        vert_after.should.eql(vert);

    });

});