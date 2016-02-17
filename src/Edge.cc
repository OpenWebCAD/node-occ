#include "Edge.h"

#include "Util.h"
#include "Mesh.h"

#include <assert.h>

bool Edge::isSeam(Base *face)
{
  if (this->shape().IsNull())
    return false;
  return BRep_Tool::IsClosed(this->edge(), TopoDS::Face(face->shape())) ? true: false;
}

bool Edge::isDegenerated()
{
  if (this->shape().IsNull())
    return true;
  return BRep_Tool::Degenerated(this->edge()) ? true: false;
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

    GC_MakeLine line(aP1, aP2);

    TopoDS_Shape shape = BRepBuilderAPI_MakeEdge(line, start->vertex(), end->vertex());
    this->setShape(shape);

  }
  CATCH_AND_RETHROW("Failed to create line");

  return 1;
}

int Edge::createArc(Vertex *start, Vertex *end, const gp_Pnt& center)
{
  try {
    gp_Pnt aP1 = start->point();
    gp_Pnt aP2 = center;
    gp_Pnt aP3 = end->point();

    Standard_Real Radius = aP1.Distance(aP2);
    gce_MakeCirc MC(aP2,gce_MakePln(aP1, aP2, aP3).Value(), Radius);
    const gp_Circ& Circ = MC.Value();

    Standard_Real Alpha1 = ElCLib::Parameter(Circ, aP1);
    Standard_Real Alpha2 = ElCLib::Parameter(Circ, aP3);
    occHandle(Geom_Circle) C = new Geom_Circle(Circ);
    occHandle(Geom_TrimmedCurve) arc = new Geom_TrimmedCurve(C, Alpha1, Alpha2, false);

    this->setShape(BRepBuilderAPI_MakeEdge(arc, start->vertex(), end->vertex()));

  }
  CATCH_AND_RETHROW("Failed to create arc");

  return 1;
}

int Edge::createArc3P(Vertex *start, Vertex *end, const gp_Pnt& aPoint)
{
  try {
    gp_Pnt aP1 = start->point();
    gp_Pnt aP2 = aPoint;
    gp_Pnt aP3 = end->point();
    GC_MakeArcOfCircle arc(aP1, aP2, aP3);
    this->setShape(BRepBuilderAPI_MakeEdge(arc, start->vertex(), end->vertex()));
  }
  CATCH_AND_RETHROW("Failed to create arc");
  return 1;
}

int Edge::createCircle(const gp_Pnt& center, const gp_Dir& normal, double radius)
{
  try {
    gp_Pnt aP1= center;
    gp_Dir aD1= normal;

    if (radius <= Precision::Confusion()) {
      StdFail_NotDone::Raise("radius to small");
    }

    gce_MakeCirc circle(aP1, aD1, radius);
    this->setShape(BRepBuilderAPI_MakeEdge(circle));

  }
  CATCH_AND_RETHROW("Failed to create circle");
  return 1;
}

Vertex* getOrCreateVertex(v8::Handle<v8::Value> arg)
{
	if (arg->IsArray()) {
		v8::Local<v8::Value> objV = Nan::New<v8::FunctionTemplate>(Vertex::_template)->GetFunction()->CallAsConstructor(1, &arg);
		if (!IsInstanceOf<Vertex>(objV)) {
			return 0;
		}
		Vertex* vertex = node::ObjectWrap::Unwrap<Vertex>(objV->ToObject());
		return vertex;
	} else if (arg->IsObject()) {
		v8::Local<v8::Value> obj = arg->ToObject();
		if (!IsInstanceOf<Vertex>(obj)) {
			return 0;
		}
		Vertex* vertex = node::ObjectWrap::Unwrap<Vertex>(obj->ToObject());
		return vertex;
  } else {
    return 0;
  }
}

NAN_METHOD(Edge::createLine)
{
  v8::Local<v8::Value> arg1 = info[0];
  v8::Local<v8::Value> arg2 = info[1];
  if (arg1.IsEmpty() || arg2.IsEmpty()) {
	return Nan::ThrowError("expecting 2 arguments : <vertex|point> , <vertex|point> ");
  }

  Vertex* v1 =     getOrCreateVertex(info[0]);
  Vertex* v2 =     getOrCreateVertex(info[1]);
  if (!v1 || !v2) {
	return Nan::ThrowError("expecting 2 arguments : <vertex|point> , <vertex|point> ");
  }

  Edge* pThis = ObjectWrap::Unwrap<Edge>(info.This());

  pThis->createLine(v1,v2);
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Edge::createCircle)
{

  v8::Local<v8::Value> arg1 = info[0];
  v8::Local<v8::Value> arg2 = info[1];
  v8::Local<v8::Value> arg3 = info[2];

  if (arg1.IsEmpty()|| arg2.IsEmpty() || arg3.IsEmpty()) {
    return Nan::ThrowError("expecting three arguments : <center>,<normal>,<radius>");
  }

  gp_Pnt center;
  ReadPoint(arg1,&center);
  gp_Dir normal;
  ReadDir(arg2,&normal);

  if (!arg3->IsNumber())  {
    return Nan::ThrowError("expecting a number (radius) as third arguments");
  }
  double radius = arg3->ToNumber()->Value();
  if (radius<1E-9)  {
    return Nan::ThrowError("radius cannot be zero ( or close to zero)");
  }

  Edge* pThis = ObjectWrap::Unwrap<Edge>(info.This());

  pThis->createCircle(center,normal,radius);

  info.GetReturnValue().Set(info.This());

}

NAN_METHOD(Edge::createArc3P)
{
  v8::Local<v8::Value> arg1 = info[0];
  v8::Local<v8::Value> arg2 = info[1];
  v8::Local<v8::Value> arg3 = info[2];

  if (arg1.IsEmpty()|| arg2.IsEmpty() || arg3.IsEmpty()) {
    return Nan::ThrowError("expecting three arguments : <center>,<normal>,<radius>");
  }

  Vertex* v1 =     getOrCreateVertex(arg1);
  gp_Pnt  p2;
  ReadPoint(arg2,&p2);
  Vertex* v3 =     getOrCreateVertex(arg3);

  Edge* pThis = ObjectWrap::Unwrap<Edge>(info.This());

  pThis->createArc3P(v1,v3,p2);

  info.GetReturnValue().Set(info.This());
}


Nan::Persistent<v8::FunctionTemplate> Edge::_template;

NAN_METHOD(Edge::New)
{
  Edge* obj = new Edge();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Object>  Edge::Clone() const
{

  Edge* obj = new Edge();
  v8::Local<v8::Object> instance = Nan::New(_template)->GetFunction()->NewInstance();
  obj->Wrap(instance);
  obj->setShape(this->shape());
  return instance;
}
void Edge::Init(v8::Handle<v8::Object> target)
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

  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Edge,length);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Edge,numVertices);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Edge,isDegenerated);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Edge,isClosed);
  EXPOSE_METHOD(Edge,createLine);
  EXPOSE_METHOD(Edge,createCircle);
  EXPOSE_METHOD(Edge,createArc3P);
  EXPOSE_METHOD(Edge,polygonize);

  target->Set(Nan::New("Edge").ToLocalChecked(), tpl->GetFunction());
}

v8::Local<v8::Object> Edge::polygonize(double factor)
{

  const TopoDS_Edge& edge = TopoDS::Edge(this->shape());

  BRepAdaptor_Curve curve_adaptor(edge);
  GCPnts_UniformDeflection discretizer;
  discretizer.Initialize(curve_adaptor, 0.05);

  m_positions.clear();
  m_positions.reserve(discretizer.NbPoints()*3);

  for (int i = 0; i < discretizer.NbPoints(); i++) {

    gp_Pnt pt = curve_adaptor.Value(discretizer.Parameter(i + 1));

    //xx std::cerr  << "[ " << pt.X() << " " << pt.Y() << " " << pt.Z() << " ] ,";
    m_positions.push_back(static_cast<float>(pt.X()));
    m_positions.push_back(static_cast<float>(pt.Y()));
    m_positions.push_back(static_cast<float>(pt.Z()));
  } 

  int length = (int)m_positions.size();
  return makeFloat32Array(m_positions.data(), length);

}


NAN_METHOD(Edge::polygonize)
{
  v8::Handle<v8::Object> pJhis = info.This();
  if ( pJhis.IsEmpty() || !IsInstanceOf<Edge>(pJhis))  {
    return Nan::ThrowError("invalid object");
  }
  Edge* pThis = node::ObjectWrap::Unwrap<Edge>(pJhis);

  double factor = 0.05;
  if (info.Length()>=1) {
    ReadDouble(info[0],factor);
  }

  info.GetReturnValue().Set(pThis->polygonize(factor));
}
