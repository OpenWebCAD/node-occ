import should from "should";
import { IEdge, IWire, Triplet, Vertex, occ } from "..";
import { getTemporaryFilePath } from "./helpers";


describe("through_sections", function () {


    it("should create a solid through a set of section", () => {

        const circle_1 = occ.makeCircle([-100., 0., -100.], [0., 0., 1.], 40.);

        const circle_2 = occ.makeCircle([-10., 0., -0.], [0., 0., 1.], 40.);

        const circle_3 = occ.makeCircle([-75., 0., 100.], [0., 0., 1.], 40.);
        const circle_4 = occ.makeCircle([0., 0., 200.], [0., 0., 1.], 40.);

        const solid = occ.makeSolidThruSections([
            circle_1, circle_2
            , circle_3, circle_4
        ]);

        const filename = getTemporaryFilePath({ prefix: "b1_", suffix: ".step" });
        console.log("file =>", filename);
        occ.writeSTEP(filename, solid);

    });

    it("should create a solid through a set of section", () => {

        const DEG2RAD = Math.atan(1) / 45;
        const r = (rho: number, theta: number): Triplet => {
            const c = Math.cos(theta * DEG2RAD);
            const s = Math.sin(theta * DEG2RAD);
            return [rho * c, rho * s, 0];
        }

        const p: Triplet[] = [];
        p.push([0, 0, 0]);
        p.push(r(50, 10));
        p.push(r(50, 8));
        p.push(r(60, 4));
        p.push(r(60, 0));
        p.push([0, 0, 0]);

        const segments: (IEdge | IWire)[] = [];
        for (let i = 0; i < p.length - 1; i++) {

            segments.push(occ.makeLine(p[i], p[i + 1]));
        }

        const wire = occ.makeWire(segments);
        const wire1 = wire.translate([0, 0, 10]).rotate([0, 0, 0,], [0, 0, 1], 6);
        const wire2 = wire.translate([0, 0, 20]).rotate([0, 0, 0,], [0, 0, 1], 12);
        const wire3 = wire.translate([0, 0, 30]).rotate([0, 0, 0,], [0, 0, 1], 18);

        const solid = occ.makeSolidThruSections([
            wire, wire1, wire2, wire3

        ]);

        const filename = getTemporaryFilePath({ prefix: "b1_", suffix: ".step" });
        console.log("file =>", filename);
        occ.writeSTEP(filename, solid);

    });

});
