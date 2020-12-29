#include "Mesh.h"

#include <set>
#include <list>
#include <algorithm>
#include <map>
#include <limits>
#include <assert.h>

#include "Util.h"
#include "TShort_Array1OfShortReal.hxx"


// old handle in V8 : see http://create.tpsitulsa.com/wiki/V8/Handles

Mesh::Mesh()
{
}

NAN_METHOD(Mesh::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowError(" use new occ.Mesh() to construct a Mesh");
  }

  Mesh* obj = new Mesh();

  Nan::Set(info.This(),Nan::New("normals").ToLocalChecked(), Nan::New<v8::Object>());
  Nan::Set(info.This(),Nan::New("edgeIndices").ToLocalChecked(), Nan::New<v8::Object>());
  Nan::Set(info.This(),Nan::New("triangles").ToLocalChecked(), Nan::New<v8::Object>());

  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

Nan::Persistent<v8::FunctionTemplate> Mesh::_template;


/*static*/
void Mesh::Init(v8::Local<v8::Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(Mesh::New);
  tpl->SetClassName(Nan::New("Mesh").ToLocalChecked());

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  EXPOSE_READ_ONLY_PROPERTY_UINT32(Mesh, numTriangles);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Mesh, numEdges);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Mesh, numVertices);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Mesh, numNormals);

  EXPOSE_METHOD(Mesh, getEdgeIndices);
  EXPOSE_METHOD(Mesh, getFaceTriangles);
  EXPOSE_METHOD(Mesh, getFaceTriangleNormals);

  // other Mesh prototype members are defined in the mesh.js script
  Nan::Set(target,Nan::New("Mesh").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());

}

long hashpoint(double x, double y, double z)
{
  long nx = static_cast<long>(x * 1000);
  long ny = static_cast<long>(y * 1000);
  long nz = static_cast<long>(z * 1000);
  return (nx * 9138553 + ny * 485993 + nz * 1194163) % 15485653;
}

uint32_t Mesh::push_point(const Coord3f& point) {
  long hash = hashpoint(point.x, point.y, point.z);
  auto it = this->_mapVertices.find(hash);
  if (it == this->_mapVertices.end()) {
    this->_vertices.push_back(point);
    uint32_t index = (uint32_t)(this->_vertices.size() - 1);
    this->_mapVertices[hash] = index;
    return index;
  }
  return it->second;
}

uint32_t Mesh::push_normal(const Coord3f& normal) {
  long hash = hashpoint(normal.x, normal.y, normal.z);
  auto it = this->_mapNormals.find(hash);
  if (it == this->_mapNormals.end()) {
    this->_normals.push_back(normal);
    uint32_t index = (uint32_t)(this->_normals.size() - 1);
    this->_mapNormals[hash] = index;
    return index;
  }
  return it->second;
}

int Mesh::extractFaceMesh(const TopoDS_Face& face, bool qualityNormals)
{

  if (face.IsNull()) {
    StdFail_NotDone::Raise("Face is Null");
  }
  auto& hashMap = this->_faceHashMap;
  int hash = face.HashCode(std::numeric_limits<int>::max());
  // find hash in edgehash.push_back(hash);
  if (hashMap.end() != hashMap.find(hash)) {
    return 0;
  }

  hashMap[hash] = (uint32_t)this->_faceRanges.size();
  this->_faceRanges.push_back((uint32_t)this->_triangles.size());

  TopLoc_Location loc;
  occHandle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, loc);
  if (triangulation.IsNull()) {
    StdFail_NotDone::Raise("No triangulation created");
  }

  bool reversed = (face.Orientation() == TopAbs_REVERSED);
  int nb_points = triangulation->NbNodes();

  gp_Trsf tr = loc;
  std::vector<uint32_t> translationTable;
  translationTable.reserve(nb_points);

  std::vector<uint32_t> normalTranslationTable;
  normalTranslationTable.reserve(nb_points);

  // ------------------------------------------------------------------------------
  // import face points into index 
  // ------------------------------------------------------------------------------
  const TColgp_Array1OfPnt&   nodes = triangulation->Nodes();
  Coord3f vert;
  double x, y, z;
  for (int i = 1; i <= nb_points; i++) {
    const gp_Pnt& pnt = nodes(i);
    x = pnt.X();
    y = pnt.Y();
    z = pnt.Z();
    tr.Transforms(x, y, z);
    vert.x = (float)x;
    vert.y = (float)y;
    vert.z = (float)z;
    uint32_t index = push_point(vert);
    translationTable.push_back(index);
  }

  // ------------------------------------------------------------------------------
  // import vertex normal
  // ------------------------------------------------------------------------------
  std::vector<gp_Vec> local_normals;
  local_normals.reserve(nb_points);

  if (!qualityNormals) {
    for (int i = 1; i <= nb_points; i++) {
      local_normals.push_back(gp_Vec(0, 0, 0));
    }
  }
  else {

    if (triangulation->HasNormals()) {

      const TShort_Array1OfShortReal& normals = triangulation->Normals();
      for (int i = 0; i < nb_points; i++) {
        const Standard_ShortReal& nx = normals( i*3 + 1);
        const Standard_ShortReal& ny = normals( i*3 + 2);
        const Standard_ShortReal& nz = normals( i*3 + 3);

        local_normals.push_back(gp_Vec(nx,ny,nz));

      }
    }
    else {

      Handle_Geom_Surface surface = BRep_Tool::Surface(face);
      gp_Vec normal;

      if (triangulation->HasUVNodes()) {

        const TColgp_Array1OfPnt2d& uvnodes = triangulation->UVNodes();
        for (int i = 0; i < nb_points; i++) {
          const gp_Pnt2d& uv = uvnodes(i + 1);
          double fU = uv.X();
          double fV = uv.Y();

          GeomLProp_SLProps faceprop(surface, fU, fV, 2, gp::Resolution());
          if (faceprop.IsNormalDefined()){
            normal = faceprop.Normal();
          }

          if (normal.SquareMagnitude() > 1.0e-10) {
            normal.Normalize();
          }
          if (reversed) {
            normal *= -1.0;
          }
          local_normals.push_back(normal);

        }
      }
      else {


        for (int i = 0; i < nb_points; i++) {

          //------------------------------------------------
          const gp_Pnt& vertex = nodes(i + 1);
          GeomAPI_ProjectPointOnSurf SrfProp(vertex, surface);
          
          Standard_Real fU, fV;
          SrfProp.Parameters(1, fU, fV);

          GeomLProp_SLProps faceprop(surface, fU, fV, 2, gp::Resolution());
          if (faceprop.IsNormalDefined()){
            normal = faceprop.Normal();
          }

          if (normal.SquareMagnitude() > 1.0e-10) {
            normal.Normalize();
          }
          if (reversed) {
            normal *= -1.0;
          }
          local_normals.push_back(normal);
        }
      }
    }
  }
  //---------------------------------------------------------------------------
  // inject normals
  //---------------------------------------------------------------------------
  for (int i = 0; i < nb_points; i++) {
    const gp_Vec n = local_normals[i];
    uint32_t index = this->push_normal(n);
    normalTranslationTable[i] = index;
  };

  //---------------------------------------------------------------------------------
  // Navigate triangles
  //---------------------------------------------------------------------------------

  const Poly_Array1OfTriangle& triarr = triangulation->Triangles();
  this->_triangles.reserve(triangulation->NbTriangles());

  Triangle3i tri;
  int nb_triangles = triangulation->NbTriangles();
  int _effective_number_of_triangle = 0;
  for (int i = 1; i <= nb_triangles; i++) {

    const Poly_Triangle& pt = triarr(i);
    Standard_Integer n1, n2, n3;
    if (reversed) {
      pt.Get(n2, n1, n3);
    }
    else {
      pt.Get(n1, n2, n3);
    }

    // make sure that we don't process invalid triangle
    if (n1 == n2 || n2 == n3 || n3 == n1)
      continue;

    // Calculate face normal
    const gp_Pnt& P1 = nodes(n1);
    const gp_Pnt& P2 = nodes(n2);
    const gp_Pnt& P3 = nodes(n3);

    gp_Vec V1(P3.X() - P1.X(), P3.Y() - P1.Y(), P3.Z() - P1.Z());
    if (V1.SquareMagnitude() < 1.0e-10) {
      // Degenerated triangle
      continue;
    }

    gp_Vec V2(P2.X() - P1.X(), P2.Y() - P1.Y(), P2.Z() - P1.Z());

    if (V2.SquareMagnitude() < 1.0e-10) {
      // Degenerated triangle
      continue;
    }

    gp_Vec normal = V1.Crossed(V2);
    if (normal.SquareMagnitude() < 1.0e-10) {
      // Degenerated triangle
      continue;
    }

    tri.i = translationTable[(uint32_t)(n1 - 1)];
    tri.j = translationTable[(uint32_t)(n2 - 1)];
    tri.k = translationTable[(uint32_t)(n3 - 1)];

    this->_triangles.push_back(tri);

    if (!qualityNormals) {

      local_normals[n1 - 1] = local_normals[n1 - 1] - normal;
      local_normals[n2 - 1] = local_normals[n2 - 1] - normal;
      local_normals[n3 - 1] = local_normals[n3 - 1] - normal;

      tri.i = this->push_normal(local_normals[n1 - 1]);
      tri.j = this->push_normal(local_normals[n2 - 1]);
      tri.k = this->push_normal(local_normals[n3 - 1]);

      this->_triangles_normals.push_back(tri);

    }
    else {
      tri.i = this->push_normal(local_normals[n1 - 1]);
      tri.j = this->push_normal(local_normals[n2 - 1]);
      tri.k = this->push_normal(local_normals[n3 - 1]);

      this->_triangles_normals.push_back(tri);

    }

    _effective_number_of_triangle++;
  }
  this->_faceRanges.push_back(_effective_number_of_triangle);

  // --------------------------------------------------------------------------
  // produce normals
  // --------------------------------------------------------------------------
  if (!qualityNormals) {
    // Normalize vertex normals
    for (int i = 0; i < nb_points; i++) {
      gp_Vec& normal = local_normals[i];
      if (normal.SquareMagnitude() > 1.0e-10) {
        normal.Normalize();
      }
    }
  }
  //---------------------------------------------------------------------------
  extractEdgeFromFace(face, triangulation, translationTable);
  return 1;
}
int Mesh::extractEdgeFromFace(const TopoDS_Face& face, const occHandle(Poly_Triangulation)& triangulation, const std::vector<uint32_t>& translationMap)
{
  TopExp_Explorer ex0, ex1;

  for (ex0.Init(face, TopAbs_WIRE); ex0.More(); ex0.Next()) {

    const TopoDS_Wire& wire = TopoDS::Wire(ex0.Current());

    for (ex1.Init(wire, TopAbs_EDGE); ex1.More(); ex1.Next()) {

      const TopoDS_Edge& edge = TopoDS::Edge(ex1.Current());

      // skip degenerated edge
      if (BRep_Tool::Degenerated(edge))
        continue;

      if (false) {
        // skip edge if it is a seam
        if (BRep_Tool::IsClosed(edge, face))
          continue;
      }
      extractEdge(edge, triangulation, translationMap);
    }
  }
  return 0;
}
int Mesh::extractEdge(const TopoDS_Edge& edge, const occHandle(Poly_Triangulation)& triangulation, const std::vector<uint32_t>& translationMap)
{

  auto& hashMap = this->_edgeHashMap;
  int hash = edge.HashCode(std::numeric_limits<int>::max());

  if (hashMap.end() != hashMap.find(hash)) {
    return 0; // already seen
  }

  TopLoc_Location loc;
  occHandle(Poly_PolygonOnTriangulation) edgepoly = BRep_Tool::PolygonOnTriangulation(edge, triangulation, loc);
  if (edgepoly.IsNull()) {
    return 0;
  }
  hashMap[hash] = (uint32_t)this->_edgeRanges.size();
  uint32_t lastSize = (uint32_t)this->_edgeIndices.size();

  this->_edgeRanges.push_back(lastSize);

  const TColStd_Array1OfInteger& edgeind = edgepoly->Nodes();
  for (int i = edgeind.Lower(); i <= edgeind.Upper(); i++) {
    const unsigned int idx = (unsigned int)edgeind(i) - 1;
    this->_edgeIndices.push_back(translationMap[idx]);
  }
  this->_edgeRanges.push_back((int)(this->_edgeIndices.size() - lastSize));
  return 1;
}

template<class T>
void UpdateExternalArray(v8::Local<v8::Object>& pThis, const char* name, const T* data, size_t _length)
{
  v8::MaybeLocal<v8::Object> arr = _makeTypedArray(data, (int)_length);
  Nan::Set(pThis,Nan::New(name).ToLocalChecked(), arr.ToLocalChecked());
}

void Mesh::updateJavaScriptArray()
{
  assert(sizeof(_triangles[0]) == sizeof(int) * 3);
  v8::Local<v8::Object> pThis = NanObjectWrapHandle(this);
  UpdateExternalArray(pThis, "vertices", &_vertices.data()[0].x, _vertices.size() * 3);
  UpdateExternalArray(pThis, "normals", &_normals.data()[0].x, _normals.size() * 3);
  UpdateExternalArray(pThis, "triangles", &_triangles.data()[0].i, _triangles.size() * 3);
  UpdateExternalArray(pThis, "triangleNormals", &_triangles_normals.data()[0].i, _triangles_normals.size() * 3);
  
  UpdateExternalArray(pThis, "faceRanges", &_faceRanges.data()[0], _faceRanges.size());
  UpdateExternalArray(pThis, "edgeIndices", &_edgeIndices.data()[0], _edgeIndices.size());
  UpdateExternalArray(pThis, "edgeRanges", &_edgeRanges.data()[0], _edgeRanges.size());
}


template <class uintX_t>
void makeTT(const std::vector<Triangle3i>& arr,uint32_t start,uint32_t length,std::vector<uintX_t>& indexes)
{
    indexes.clear();
    indexes.reserve(length * 3);
    for (uint32_t i = 0; i < length; i++) {
      indexes.push_back((uintX_t)arr[start + i].i);
      indexes.push_back((uintX_t)arr[start + i].j);
      indexes.push_back((uintX_t)arr[start + i].k);
    }
}
template <class uintX_t>
void makeTT(const std::vector<uint32_t>& arr,uint32_t start,uint32_t length,std::vector<uintX_t>& indexes)
{
    indexes.clear();
    indexes.reserve(length);
    for (uint32_t i = 0; i < length; i++) {
      indexes.push_back(arr[start + i]);
    }
}

#include "Face.h"
NAN_METHOD(Mesh::getFaceTriangles)
{
  Mesh* pThis = UNWRAP(Mesh);
  Face* pFace = 0;
  if (info.Length() == 1 && info[0]->IsObject()) {
    pFace = Nan::ObjectWrap::Unwrap<Face>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
  }
  if (!pFace) {
    return Nan::ThrowError("expecting one argument : face");
  }
  auto& hashMap = pThis->_faceHashMap;
  int hash = pFace->m_face.HashCode(std::numeric_limits<int>::max());
  // find hash in edgehash.push_back(hash);
  if (hashMap.end() == hashMap.find(hash)) {
    return Nan::ThrowError("Cannot find indices for this face");
  }
  uint32_t index = hashMap[hash];
  uint32_t start = pThis->_faceRanges[index];
  uint32_t length = pThis->_faceRanges[index + 1];
  const auto arr = pThis->_triangles;

  if (pThis->numVertices() <= 255) {
    std::vector<uint8_t> indexes;
    makeTT<uint8_t>(arr,start,length,indexes);
    info.GetReturnValue().Set(makeUint8Array(indexes.data(), length * 3));
    return;
  }
  if (pThis->numVertices() <= 65535) {
    std::vector<uint16_t> indexes;
    makeTT<uint16_t>(arr,start,length,indexes);
    info.GetReturnValue().Set(makeUint16Array(indexes.data(), length * 3));
    return;
  }
  std::vector<uint32_t> indexes;
  makeTT<uint32_t>(arr,start,length,indexes);
  info.GetReturnValue().Set(makeUint32Array(indexes.data(), length * 3));
}

NAN_METHOD(Mesh::getFaceTriangleNormals)
{
  Mesh* pThis = UNWRAP(Mesh);
  Face* pFace = 0;
  if (info.Length() == 1 && info[0]->IsObject()) {
    pFace = Nan::ObjectWrap::Unwrap<Face>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
  }
  if (!pFace) {
    return Nan::ThrowError("expecting one argument : face");
  }
  auto& hashMap = pThis->_faceHashMap;
  int hash = pFace->m_face.HashCode(std::numeric_limits<int>::max());
  // find hash in edgehash.push_back(hash);
  if (hashMap.end() == hashMap.find(hash)) {
    return Nan::ThrowError("Cannot find indices for this face");
  }
  uint32_t index = hashMap[hash];
  uint32_t start = pThis->_faceRanges[index];
  uint32_t length = pThis->_faceRanges[index + 1];

  const auto arr = pThis->_triangles_normals;

  if (pThis->numNormals() <= 255) {
    std::vector<uint8_t> indexes;
    makeTT<uint8_t>(arr,start,length,indexes);
    info.GetReturnValue().Set(makeUint8Array(indexes.data(), length * 3));
    return;
  }
  if (pThis->numNormals() <= 65535) {
    std::vector<uint16_t> indexes;
    makeTT<uint16_t>(arr,start,length,indexes);
    info.GetReturnValue().Set(makeUint16Array(indexes.data(), length * 3));
    return;
  }
  std::vector<uint32_t> indexes;
  makeTT<uint32_t>(arr,start,length,indexes);
  info.GetReturnValue().Set(makeUint32Array(indexes.data(), length * 3));
}
#include "Edge.h"
/**
 * @param edge {Edge} the edge from which we want to get the indices
 * @return {Int32Array}
 */
NAN_METHOD(Mesh::getEdgeIndices)
{
  Mesh* pThis = UNWRAP(Mesh);
  Edge* pEdge = 0;
  if (info.Length() == 1 && info[0]->IsObject()) {
    pEdge = Nan::ObjectWrap::Unwrap<Edge>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
  }
  if (!pEdge) {
    return Nan::ThrowError("expecting one argument : edge");
  }

  int hash = pEdge->m_edge.HashCode(std::numeric_limits<int>::max());

  auto& hashMap = pThis->_edgeHashMap;

  // find hash in edgehash.push_back(hash);
  if (hashMap.end() == hashMap.find(hash)) {
    return Nan::ThrowError("Cannot find indices for this edge");
  }
  uint32_t index = hashMap[hash];

  uint32_t start = pThis->_edgeRanges[index];
  uint32_t length = pThis->_edgeRanges[index + 1];
  const auto arr = pThis->_edgeIndices;


  if (pThis->numVertices() <= 255) {
    std::vector<uint8_t> indexes;
    makeTT<uint8_t>(arr,start,length,indexes);
    info.GetReturnValue().Set(makeUint8Array(indexes.data(), length));
    return;
  }
  if (pThis->numVertices() <= 65535) {
    std::vector<uint16_t> indexes;
    makeTT<uint16_t>(arr,start,length,indexes);
    info.GetReturnValue().Set(makeUint16Array(indexes.data(), length));
    return;
  }

  std::vector<uint32_t> indexes;
  makeTT<uint32_t>(arr,start,length,indexes);
  info.GetReturnValue().Set(makeUint32Array(indexes.data(), length));

}

float square(float b)
{
  return b*b;
}

float distance2(const Coord3f& a, const Coord3f& b)
{
  return square(a.x - b.x) + square(a.y - b.y) + square(a.y - b.y);
}
int deprecated_findPt(const std::vector<Coord3f>& arrayPts, const Coord3f pt, double tolerance)
{

  double tolerance2 = tolerance*tolerance;
  int i = 0;
  for (std::vector<Coord3f>::const_iterator it = arrayPts.begin(); it != arrayPts.end(); it++) {
    const Coord3f& point = (*it);
    if (distance2(point, pt) < tolerance2) {
      return i;
    }
    i++;
  }
  return -1;
}
void Mesh::optimize()
{
  updateJavaScriptArray();
}
// see also :
// https://github.com/FreeCAD/FreeCAD/src/3rdParty/salomesmesh/src/StdMeshers/StdMeshers_Adaptive1D.cpp
