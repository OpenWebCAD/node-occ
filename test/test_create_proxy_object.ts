
import should from "should";
import { calculateOperationHash, createProxyObject } from "../dist/fastbuilder";

describe("testing calculateOperationHash", function () {

    it("should calculate the hash of 10,20,30", function () {
        calculateOperationHash("myFunc", [10, 20, 30], false)[1].should.equal("myFunc(10,20,30)");
    });
    it("should calculate the hash of [10,20,30]", function () {
        calculateOperationHash("myFunc", [[10, 20, 30]], false)[1].should.equal("myFunc([10,20,30])");
    });
    it("should calculate the hash of [10,20,30], \"Hello\"", function () {
        calculateOperationHash("myFunc", [[10, 20, 30], "Hello"], false)[1].should.equal("myFunc([10,20,30],\"Hello\")");
    });
});

describe("testing createProxyObject", function () {






    it("CF1 should ", () => {

        const a = {
            add(a1: number, a2: number): number {
                return a1 + a2;
            }
        }

        const b = createProxyObject(a);

        const resultFromB = b.add(1, 2);

        const resultFromA = a.add(1, 2);

        should(resultFromA).eql(resultFromB);
    })

});