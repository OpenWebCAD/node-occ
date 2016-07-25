#include "Vertex.h"

#include "Util.h"

const TopoDS_Shape& Vertex::shape()  const
{
  return vertex();
}

void Vertex::setShape(const TopoDS_Shape& shape)
{
  m_vertex = TopoDS::Vertex(shape);
}

gp_Pnt Vertex::point() const
{
  gp_Pnt pnt = BRep_Tool::Pnt(vertex());
  return pnt;
}
double Vertex::x()
{
  return  point().X();
}
double Vertex::y()
{
  return  point().Y();
}
double Vertex::z()
{
  return  point().Z();
}

Nan::Persistent<v8::FunctionTemplate> Vertex::_template;


NAN_METHOD(Vertex::NewInstance) { _NewInstance<Vertex>(info); }

NAN_METHOD(Vertex::New)
{
  if (!info.IsConstructCall()) {
   return Nan::ThrowError(" use new occ.Vertex() to construct a Vertex");
  }

  Vertex* pThis = new Vertex();
  pThis->Wrap(info.This());
  pThis->InitNew(info);

  // return scope.Close(args.This());
  double x = 0, y = 0, z = 0;
  if (info.Length() == 3) {
    ReadDouble(info[0], x);
    ReadDouble(info[1], y);
    ReadDouble(info[2], z);
  }
  else if (info.Length() == 1) {
    ReadPoint(info[0], &x, &y, &z);
  }
  else if (info.Length() == 0) {
  }
  else {
    return Nan::ThrowError("Wrong Arguments");
  }
  gp_Pnt aPnt;
  aPnt = gp_Pnt(x, y, z);
  BRepBuilderAPI_MakeVertex mkVertex(aPnt);
  pThis->setShape(mkVertex.Vertex());

  info.GetReturnValue().Set(info.This());
}


v8::Local<v8::Object>  Vertex::Clone() const
{
  Vertex* obj = new Vertex();
  v8::Local<v8::Object> instance = Nan::New(_template)->GetFunction()->NewInstance(Nan::GetCurrentContext()).ToLocalChecked();
  obj->Wrap(instance);
  obj->setShape(this->shape());
  return instance;
}

void Vertex::InitNew(_NAN_METHOD_ARGS)
{
  Base::InitNew(info);
  REXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex,x);
  REXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex,y);
  REXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex,z);
}

void Vertex::Init(v8::Handle<v8::Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate>  tpl = Nan::New<v8::FunctionTemplate>(Vertex::New);
  tpl->SetClassName(Nan::New("Vertex").ToLocalChecked());

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  Base::InitProto(proto);

  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex, x);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex, y);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex, z);

  target->Set(Nan::New("Vertex").ToLocalChecked(), tpl->GetFunction());
}

