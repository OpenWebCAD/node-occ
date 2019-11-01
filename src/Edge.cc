#include "Edge.h"

#include "Util.h"
#include "Mesh.h"

#include <assert.h>


#define EXPOSE_POINT_PROPERTY(THISTYPE,ACCESSOR)                                            \
  Nan::SetAccessor(proto,                                                            \
			Nan::New<v8::String>(#ACCESSOR).ToLocalChecked(),                        \
					&THISTYPE::getter_##ACCESSOR,  0,v8::Local<v8::Value>(),v8::DEFAULT,(v8::PropertyAttribute)(v8::ReadOnly|v8::DontDelete))

#define REXPOSE_POINT_PROPERTY(THISTYPE,ACCESSOR)                                            \
  Nan::SetAccessor(info.This(),                                                            \
			Nan::New<v8::String>(#ACCESSOR).ToLocalChecked(),                        \
					&THISTYPE::getter_##ACCESSOR,  0,v8::Local<v8::Value>(),v8::DEFAULT,(v8::PropertyAttribute)(v8::ReadOnly|v8::DontDelete))

bool Edge::isSeam(Base *face)
{
  if (this->shape().IsNull())
    return false;
  return BRep_Tool::IsClosed(this->edge(), TopoDS::Face(face->shape())) ? true : false;
}

bool Edge::isDegenerated()
{
  if (this->shape().IsNull())
    return true;
  return BRep_Tool::Degenerated(this->edge()) ? true : false;
}

bool Edge::isClosed()
{
  if (this->shape().IsNull())
    return false;
  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(this->edge(), aV1, aV2);
  if (!aV1.IsNull() && !aV2.IsNull() && aV1.IsSame(aV2))
    return true;
  return false;
}

int Edge::numVertices()
{
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(this->edge(), TopAbs_VERTEX, anIndices);
  return anIndices.Extent();
}

double Edge::length()
{
  if (edge().IsNull()) return 0.0;

  GProp_GProps prop;
  BRepGProp::LinearProperties(this->edge(), prop);
  return prop.Mass();
}



int Edge::createLine(Vertex *start, Vertex *end)
{

  try {
    gp_Pnt aP1 = start->point();
    gp_Pnt aP2 = end->point();

    gce_MakeLin mLine(aP1, aP2);
    const gp_Lin& line= mLine.Value();

    TopoDS_Shape shape = BRepBuilderAPI_MakeEdge(line, start->vertex(), end->vertex());
    this->setShape(shape);

  }
  CATCH_AND_RETHROW_NO_RETURN("Failed to create line");

  return 1;
}

int Edge::createArc(Vertex *start, Vertex *end, const gp_Pnt& center)
{
  try {
    gp_Pnt aP1 = start->point();
    gp_Pnt aP2 = center;
    gp_Pnt aP3 = end->point();

    Standard_Real Radius = aP1.Distance(aP2);
    gce_MakeCirc MC(aP2, gce_MakePln(aP1, aP2, aP3).Value(), Radius);
    const gp_Circ& Circ = MC.Value();

    Standard_Real Alpha1 = ElCLib::Parameter(Circ, aP1);
    Standard_Real Alpha2 = ElCLib::Parameter(Circ, aP3);
    occHandle(Geom_Circle) C = new Geom_Circle(Circ);
    occHandle(Geom_TrimmedCurve) arc = new Geom_TrimmedCurve(C, Alpha1, Alpha2, false);

    this->setShape(BRepBuilderAPI_MakeEdge(arc, start->vertex(), end->vertex()));

  }
  CATCH_AND_RETHROW_NO_RETURN("Failed to create arc");
  return 1;
}

int Edge::createArc3P(Vertex *start, Vertex *end, const gp_Pnt& aPoint)
{
  try {
    gp_Pnt aP1 = start->point();
    gp_Pnt aP2 = aPoint;
    gp_Pnt aP3 = end->point();
    GC_MakeArcOfCircle arc(aP1, aP2, aP3);
    this->setShape(BRepBuilderAPI_MakeEdge(arc.Value(), start->vertex(), end->vertex()));
  }
  CATCH_AND_RETHROW_NO_RETURN("Failed to create arc");
  return 1;
}

int Edge::createCircle(const gp_Pnt& center, const gp_Dir& normal, double radius)
{
  try {
    gp_Pnt aP1 = center;
    gp_Dir aD1 = normal;

    if (radius <= Precision::Confusion()) {
      StdFail_NotDone::Raise("radius to small");
    }

    gce_MakeCirc circle(aP1, aD1, radius);
    this->setShape(BRepBuilderAPI_MakeEdge(circle));

  }
  CATCH_AND_RETHROW_NO_RETURN("Failed to create circle");
  return 1;
}

template <class T> T* my_unwrap(v8::MaybeLocal<v8::Value> value) {
  
  auto a = Nan::To<v8::Object>(value.ToLocalChecked()).ToLocalChecked();
  return Nan::ObjectWrap::Unwrap<T>(a);
}

Vertex* getOrCreateVertex(v8::Local<v8::Value> arg)
{
  Nan::HandleScope scope;

  if (arg->IsArray()) {
    auto objV = Nan::NewInstance(Constructor<Vertex>(), 1, &arg).ToLocalChecked();
    if (!IsInstanceOf<Vertex>(objV)) {
      return 0;
    }
    Vertex* vertex = my_unwrap<Vertex>(objV);
    return vertex;
  }
  else if (arg->IsObject()) {

    v8::Local<v8::Value> obj = Nan::To<v8::Object>(arg).ToLocalChecked();
    if (!IsInstanceOf<Vertex>(obj)) {
      return 0;
    }
    Vertex* vertex = Nan::ObjectWrap::Unwrap<Vertex>(Nan::To<v8::Object>(obj).ToLocalChecked());
    return vertex;
  }
  else {
    return 0;
  }
}

Nan::Persistent<v8::FunctionTemplate> Edge::_template;

NAN_METHOD(Edge::static_createLine)
{
  v8::Local<v8::Value> arg1 = info[0];
  v8::Local<v8::Value> arg2 = info[1];
  if (arg1.IsEmpty() || arg2.IsEmpty()) {
    return Nan::ThrowError("expecting 2 arguments : <vertex|point> , <vertex|point> ");
  }

  Vertex* v1 = getOrCreateVertex(info[0]);
  Vertex* v2 = getOrCreateVertex(info[1]);
  if (!v1 || !v2) {
    return Nan::ThrowError("expecting 2 arguments : <vertex|point> , <vertex|point> ");
  }

  v8::Local<v8::Object> instance = Nan::NewInstance(Constructor<Edge>()).ToLocalChecked();
  Edge* pThis = Nan::ObjectWrap::Unwrap<Edge>(Nan::To<v8::Object>(instance).ToLocalChecked());
  
  pThis->createLine(v1, v2);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Edge::static_createCircle)
{

  v8::Local<v8::Value> arg1 = info[0];
  v8::Local<v8::Value> arg2 = info[1];
  v8::Local<v8::Value> arg3 = info[2];

  if (arg1.IsEmpty() || arg2.IsEmpty() || arg3.IsEmpty()) {
    return Nan::ThrowError("expecting three arguments : <center>,<normal>,<radius>");
  }

  gp_Pnt center;
  ReadPoint(arg1, &center);
  gp_Dir normal;
  ReadDir(arg2, &normal);

  if (!arg3->IsNumber()) {
    return Nan::ThrowError("expecting a number (radius) as third arguments");
  }
  double radius;
  ReadDouble(arg3, radius);

  if (radius < 1E-9) {
    return Nan::ThrowError("radius cannot be zero ( or close to zero)");
  }


  Edge* pThis = new Edge();
  v8::Local<v8::Object> instance = makeInstance(_template);
  pThis->Wrap(instance);

  pThis->createCircle(center, normal, radius);

  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Edge::static_createArc3P)
{

  v8::Local<v8::Value> arg1 = info[0];
  v8::Local<v8::Value> arg2 = info[1];
  v8::Local<v8::Value> arg3 = info[2];

  if (arg1.IsEmpty() || arg2.IsEmpty() || arg3.IsEmpty()) {
    return Nan::ThrowError("expecting three arguments : <center>,<normal>,<radius>");
  }


  Vertex* v1 = getOrCreateVertex(arg1);
  gp_Pnt  p2;
  ReadPoint(arg2, &p2);
  Vertex* v3 = getOrCreateVertex(arg3);


  Edge* pThis = new Edge();
  v8::Local<v8::Object> instance = makeInstance(_template);
  pThis->Wrap(instance);

  pThis->createArc3P(v1, v3, p2);

  info.GetReturnValue().Set(instance);
}


NAN_METHOD(Edge::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowError(" use new occ.Edge() to construct a Edge");
  }
  Edge* pThis = new Edge();
  pThis->Wrap(info.This());
  pThis->InitNew(info);

  info.GetReturnValue().Set(info.This());

  REXPOSE_READ_ONLY_PROPERTY_DOUBLE(Edge, length);
  REXPOSE_READ_ONLY_PROPERTY_INTEGER(Edge, numVertices);
  REXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Edge, isDegenerated);
  REXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Edge, isClosed);
  REXPOSE_POINT_PROPERTY(Edge, firstVertex);
  REXPOSE_POINT_PROPERTY(Edge, lastVertex);

}



v8::Local<v8::Object>  Edge::Clone() const
{

  Edge* obj = new Edge();
  v8::Local<v8::Object> instance = makeInstance(_template);
  obj->Wrap(instance);
  obj->setShape(this->shape());
  return instance;
}

NAN_PROPERTY_GETTER(Edge::getter_firstVertex) {

  if (info.This().IsEmpty()) {
    info.GetReturnValue().SetUndefined();
    return;
  }

  if (info.This()->InternalFieldCount() == 0) {
    info.GetReturnValue().SetUndefined();
    return;
  }

  Edge* pThis = Nan::ObjectWrap::Unwrap<Edge>(info.This());

  TopoDS_Vertex shape = TopExp::FirstVertex(pThis->edge()/*,CumOri=false*/);

  v8::Local<v8::Object> obj = buildWrapper(shape);
  info.GetReturnValue().Set(obj);

}
NAN_PROPERTY_GETTER(Edge::getter_lastVertex) {

  if (info.This().IsEmpty()) {
    info.GetReturnValue().SetUndefined();
    return;
  }

  if (info.This()->InternalFieldCount() == 0) {
    info.GetReturnValue().SetUndefined();
    return;
  }

  Edge* pThis = Nan::ObjectWrap::Unwrap<Edge>(info.This());

  TopoDS_Vertex shape = TopExp::LastVertex(pThis->edge()/*,CumOri=false*/);

  v8::Local<v8::Object> obj = buildWrapper(shape);
  info.GetReturnValue().Set(obj);

}





void Edge::Init(v8::Local<v8::Object> target)
{

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(Edge::New);
  tpl->SetClassName(Nan::New("Edge").ToLocalChecked());


  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  Base::InitProto(proto);

  EXPOSE_METHOD(Edge, getVertices);

  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Edge, length);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Edge, numVertices);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Edge, isDegenerated);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Edge, isClosed);
  EXPOSE_POINT_PROPERTY(Edge, firstVertex);
  EXPOSE_POINT_PROPERTY(Edge, lastVertex);


  EXPOSE_METHOD(Edge, polygonize);
  //xx  EXPOSE_METHOD(Edge, polygonOnTriangulation);


  Nan::Set(target, Nan::New("Edge").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());

  //xx EXPOSE_STATIC_METHOD(Edge,createLine);
  //xx EXPOSE_STATIC_METHOD(Edge,createCircle);
  //xx EXPOSE_STATIC_METHOD(Edge,createArc3P);
  Nan::SetMethod(tpl, "makeLine", Edge::static_createLine);
  Nan::SetMethod(tpl, "makeArc3P", Edge::static_createArc3P);
  Nan::SetMethod(tpl, "makeCircle", Edge::static_createCircle);

  Nan::SetMethod(target, "makeLine", Edge::static_createLine);
  Nan::SetMethod(target, "makeArc3P", Edge::static_createArc3P);
  Nan::SetMethod(target, "makeCircle", Edge::static_createCircle);

}


void extractEdgePolygon(const TopoDS_Edge& edge, std::vector<float>& positions)
{
  if (edge.IsNull()) {
    StdFail_NotDone::Raise("Face is Null");
  }
  TopLoc_Location loc;
  occHandle(Poly_Polygon3D) polygon = BRep_Tool::Polygon3D(edge, loc);
  if (polygon.IsNull()) {
    StdFail_NotDone::Raise("cannot find  Poly_Polygon3D on edge with BRep_Tool::Polygon3Dated");
  }

  //xx if (polygon.Deflection() == factor)
  const  TColgp_Array1OfPnt& points = polygon->Nodes();
  positions.clear();

  int n = points.Length();
  positions.reserve(n * 3);
  for (int i = 0; i < n; i++) {
    gp_Pnt pt = points(i);
    positions.push_back(static_cast<float>(pt.X()));
    positions.push_back(static_cast<float>(pt.Y()));
    positions.push_back(static_cast<float>(pt.Z()));
  }
  return;

}

v8::Local<v8::Object> Edge::polygonize(double factor)
{

  const TopoDS_Edge& edge = TopoDS::Edge(this->shape());

  if (factor == 0.0) {
    extractEdgePolygon(edge, m_positions);
    int length = (int)m_positions.size();
    return makeFloat32Array(m_positions.data(), length);
  }

  BRepAdaptor_Curve curve_adaptor(edge);
  GCPnts_UniformDeflection discretizer;
  discretizer.Initialize(curve_adaptor, 0.05);

  m_positions.clear();
  m_positions.reserve(discretizer.NbPoints() * 3);
  for (int i = 0; i < discretizer.NbPoints(); i++) {
    gp_Pnt pt = curve_adaptor.Value(discretizer.Parameter(i + 1));
    m_positions.push_back(static_cast<float>(pt.X()));
    m_positions.push_back(static_cast<float>(pt.Y()));
    m_positions.push_back(static_cast<float>(pt.Z()));
  }
  int length = (int)m_positions.size();
  return makeFloat32Array(m_positions.data(), length);

}

NAN_METHOD(Edge::getVertices)
{
  Edge* pThis = UNWRAP(Edge);
  auto arr = extract_shapes_as_javascript_array(pThis, TopAbs_VERTEX);
  info.GetReturnValue().Set(arr);
}


NAN_METHOD(Edge::polygonize)
{
  Edge* pThis = UNWRAP(Edge);

  if (info.Length() == 0) {

    try {
      const TopoDS_Edge& edge = TopoDS::Edge(pThis->shape());
      extractEdgePolygon(edge, pThis->m_positions);
      int length = (int)pThis->m_positions.size();
      info.GetReturnValue().Set(makeFloat32Array(pThis->m_positions.data(), length));
      return;
    }
    catch (Standard_Failure& e) {
      const Standard_CString msg = e.GetMessageString();
      if (msg != NULL && strlen(msg) > 1) {
        //xx cout << " message =" << msg << "\n";// setErrorMessage(msg);
      }
      else {
        //xx cout << " message =" << "failed to polygonize edge ( extracting default )" << "\n";// setErrorMessage(msg);
      }
    }
  }
  double factor = 0.5;
  if (info.Length() >= 1) {
    ReadDouble(info[0], factor);
  }
  v8::Local<v8::Object> ret = pThis->polygonize(factor);
  info.GetReturnValue().Set(ret);
}

