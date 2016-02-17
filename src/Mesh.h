#pragma once
#include "NodeV8.h"
#include "GeometryBuilder.h"
#include <vector>

struct Coord3f {
    float x;
    float y;
    float z;
};
struct Triangle3i {
    int i;
    int j;
    int k;
};
class Mesh : public  node::ObjectWrap  {
public:
    Mesh();
    int extractFaceMesh(const TopoDS_Face& face, bool qualityNormals);
    void optimize();

    static NAN_METHOD(New);

    static void Init(v8::Handle<v8::Object> target);


private:
    std::vector<Coord3f> normals;
    std::vector<Coord3f> vertices;
    std::vector<Triangle3i> triangles;
    std::vector<unsigned int> edgeindices;
    std::vector<int> edgeranges;
    std::vector<int> edgehash;
    friend class MeshOptimizer;

    void updateJavaScriptArray();
public:
    static Nan::Persistent<v8::FunctionTemplate> _template;
    int32_t numTriangles()  {
        return  (int32_t) triangles.size();
    }
    int32_t numVertices()  {
        return (int32_t)vertices.size();
    }
    int32_t numNormals()  {
        return (int32_t)normals.size();
    }
    int32_t numEdges()  {
        return (int32_t)edgeindices.size();
    }

    void setErrorMessage(const char* message) {};

};
