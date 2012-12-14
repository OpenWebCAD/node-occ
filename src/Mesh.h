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
class Mesh : public Shape {
public:
  Mesh();
  int extractFaceMesh(const TopoDS_Face& face, bool qualityNormals);
  void optimize();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static void Init(v8::Handle<v8::Object> target);
private:
  std::vector<Coord3f> normals;
  std::vector<Coord3f> vertices;
  std::vector<Triangle3i> triangles;
  std::vector<unsigned int> edgeindices;
  std::vector<int> edgeranges;
  std::vector<int> edgehash;
  friend class MeshOptimizer;
public:
 static v8::Persistent<v8::Function> constructor;
  size_t numTriangles()  { return triangles.size(); }
  size_t numVertices()  { return vertices.size(); }
  size_t numEdges()  { return edgeindices.size(); }
};
