const assert = require("assert");
const occ = require("./bindings");


function toArray(meshBuffer) {
    const res = [];
    for (let i = 0; i < meshBuffer.length; i++) {
        res.push(parseFloat(meshBuffer[i].toFixed(3)));
    }
    return res;
}
exports.init = function () {


    const hasFaceVertexNormal = (1 << 5);
    // hasFaceNormal would be (1 << 4);
    occ.Mesh.prototype.toThreeJS_JSONFormat = function () {

        // see https://github.com/mrdoob/three.js/wiki/JSON-Model-format-3.1
        // https://github.com/mrdoob/three.js/blob/102497da4213b9fceae9fd9bf87a37f7574ba387/src/loaders/JSONLoader.js
        const json = {
            metadata: {"formatVersion": 3},
            materials: [{
                DbgColor: 15658734, // => 0xeeeeee
                DbgIndex: 0,
                DbgName: "dummy",
                colorDiffuse: [1, 0, 0]
            }],
            vertices: [],
            normals: [],
            faces: []
        };

        json.vertices = toArray(this.vertices);
        json.normals = toArray(this.normals);
        json.uvs = [];
        json.faces = [];

        for (let i = 0; i < this.numTriangles; i++) {

            json.faces.push(hasFaceVertexNormal); // vertex + normal index

            const i1 = this.triangles[i * 3 + 0];
            const i2 = this.triangles[i * 3 + 1];
            const i3 = this.triangles[i * 3 + 2];

            const j1 = this.triangleNormals[i * 3 + 0];
            const j2 = this.triangleNormals[i * 3 + 1];
            const j3 = this.triangleNormals[i * 3 + 2];

            // face triangle vertex indexes
            json.faces.push(i1);
            json.faces.push(i2);
            json.faces.push(i3);

            // face vertex normal indexes
            json.faces.push(j1);
            json.faces.push(j2);
            json.faces.push(j3);
        }
        return json;
    };

    occ.Mesh.prototype.toJSON = occ.Mesh.prototype.toThreeJS_JSONFormat;

};
