#pragma once
#include "NodeV8.h"
#include "GeometryBuilder.h"
#include <vector>
#include <map>

struct Coord3f {
    float x;
    float y;
    float z;
    Coord3f(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}
    Coord3f() :x(0.0), y(0.0), z(0.0) {}
    Coord3f(const gp_Vec& vec) : x((float)vec.X()), y((float)vec.Y()), z((float)vec.Z()) {}
};

struct Triangle3i {
  uint32_t i;
  uint32_t j;
  uint32_t k;
};
class Mesh : public  Nan::ObjectWrap  {
  int extractEdge(const TopoDS_Edge& edge, const occHandle(Poly_Triangulation)& triangulation, const std::vector<uint32_t>& translationMap);
  int extractEdgeFromFace(const TopoDS_Face& face, const occHandle(Poly_Triangulation)& triangulation, const std::vector<uint32_t>& translationMap);
public:
    Mesh();
    int extractFaceMesh(const TopoDS_Face& face, bool qualityNormals);

    void optimize();

    static NAN_METHOD(New);

    static NAN_METHOD(getEdgeIndices);
    static NAN_METHOD(getFaceTriangles);
    static NAN_METHOD(getFaceTriangleNormals);

    static void Init(v8::Local<v8::Object> target);

private:

    // a set of points to support edges and triangles
    std::vector<Coord3f>     _vertices;
    // a set of normals 
    std::vector<Coord3f>     _normals;

    std::vector<Triangle3i>   _triangles;
    std::vector<Triangle3i>   _triangles_normals;

    std::vector<uint32_t>  _edgeIndices;

    // an index to quickly find the index of a vertex given its x,y,z coordinates
    std::map<long, uint32_t>      _mapVertices;
    // an index to quickly find the index of a vertex given its x,y,z coordinates
    std::map<long, uint32_t>      _mapNormals;


    // -----------------------------------------------------------------------
    std::vector<uint32_t>        _faceRanges;     // [start(0),nb(0),....,start(i),nb(i)]
    std::map<int, uint32_t>      _faceHashMap;    // hash,index

    std::vector<uint32_t>        _edgeRanges;     // [start(0),nb(0),....,start(i),nb(i)]
    std::map<int, uint32_t>      _edgeHashMap;    // hash,index

    friend class MeshOptimizer;
    uint32_t push_point(const Coord3f& point);
    uint32_t push_normal(const Coord3f& normal);
    void updateJavaScriptArray();
public:
    static Nan::Persistent<v8::FunctionTemplate> _template;

    uint32_t numTriangles()  {
        return  (uint32_t) _triangles.size();
    }
    
    int32_t numVertices()  {
        return (int32_t) _vertices.size();
    }
    
    int32_t numNormals()  {
        return (int32_t) _normals.size();
    }
    
    int32_t numEdges()  {
        return (int32_t)  _edgeIndices.size()>>1;
    }
    void setErrorMessage(const char* message) {
    };
};
