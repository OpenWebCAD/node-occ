const assert = require('assert');
const occ = require('./bindings');


function toArray(meshBuffer) {
    let res = [];
    for (let i = 0; i < meshBuffer.length; i++) {
        res.push(parseFloat(meshBuffer[i].toFixed(3)));
    }
    return res;
}
exports.init = function () {


    occ.Mesh.prototype.toJSON = function () {

        // see https://github.com/mrdoob/three.js/wiki/JSON-Model-format-3.1
        let json = {
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

        let f;
        let j = 0;
        for (let i = 0; i < this.numTriangles; i++) {

            json.faces.push(0 + 2 * 2 * 2 * 2 * 2); // vertex + normal index

            let i1 = this.triangles[i * 3 + 0];
            let i2 = this.triangles[i * 3 + 1];
            let i3 = this.triangles[i * 3 + 2];

            json.faces.push(i1);
            json.faces.push(i2);
            json.faces.push(i3);
            json.faces.push(i1);
            json.faces.push(i2);
            json.faces.push(i3);
        }
        return json;
    };


};
