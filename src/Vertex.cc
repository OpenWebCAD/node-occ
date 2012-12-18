#include "Vertex.h"

#include "Util.h"

const TopoDS_Shape& Vertex::shape()  const
{
    return vertex();
}

void Vertex::setShape( const TopoDS_Shape& shape)
{
  m_vertex = TopoDS::Vertex(shape);
}

gp_Pnt Vertex::point() const 
{
    gp_Pnt pnt = BRep_Tool::Pnt(vertex());
	return pnt;
}
double Vertex::x()  {	return  point().X(); }
double Vertex::y()  {	return  point().Y(); }
double Vertex::z()  {	return  point().Z(); }

Persistent<FunctionTemplate> Vertex::constructor;

Handle<Value> Vertex::New(const Arguments& args)
{
  HandleScope scope;
  
  Vertex* pThis = new Vertex();
  pThis->Wrap(args.This());
  // return scope.Close(args.This());
  double x=0,y=0,z=0;
  if (args.Length() == 3) {
	  x = args[0]->ToNumber()->Value();
	  y = args[1]->ToNumber()->Value();
	  z = args[2]->ToNumber()->Value();
  } else if (args.Length() == 1) {
	  ReadPoint(args[0],&x,&y,&z);
  } else if (args.Length() == 0) {
  } else {
	 ThrowException(Exception::TypeError(String::New("Wrong Arguments")));
  }
  gp_Pnt aPnt;
  aPnt = gp_Pnt(x, y, z);
  BRepBuilderAPI_MakeVertex mkVertex(aPnt);  
  pThis->setShape(mkVertex.Vertex());

  return args.This();
}
void Vertex::Init(Handle<Object> target)
{
  // Prepare constructor template
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Vertex::New));
  constructor->SetClassName(String::NewSymbol("Vertex"));

  // object has one internal filed ( the C++ object)
  constructor->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

  Base::InitProto(proto);

  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex,x);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex,y);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex,z);

  target->Set(String::NewSymbol("Vertex"), constructor->GetFunction());
}