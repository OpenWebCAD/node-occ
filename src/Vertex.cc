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

Persistent<FunctionTemplate> Vertex::_template;

NAN_METHOD(Vertex::New)
{
    NanScope();

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
        NanThrowError("Wrong Arguments");
        NanReturnUndefined();
    }
    gp_Pnt aPnt;
    aPnt = gp_Pnt(x, y, z);
    BRepBuilderAPI_MakeVertex mkVertex(aPnt);
    pThis->setShape(mkVertex.Vertex());

    NanReturnValue(args.This());
}


Local<Object>  Vertex::Clone() const
{
    Vertex* obj = new Vertex();
    Local<Object> instance = NanNew(_template)->GetFunction()->NewInstance();
    obj->Wrap(instance);
    obj->setShape(this->shape());
    return instance;
}
void Vertex::Init(Handle<Object> target)
{
    // Prepare constructor template
      v8::Local<v8::FunctionTemplate>  tpl = NanNew<v8::FunctionTemplate>(Vertex::New);
      tpl->SetClassName(NanNew("Vertex"));

    // object has one internal filed ( the C++ object)
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NanAssignPersistent<v8::FunctionTemplate>(_template, tpl);

    // Prototype
    Local<ObjectTemplate> proto = tpl->PrototypeTemplate();

    Base::InitProto(proto);

    EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex,x);
    EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex,y);
    EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Vertex,z);

    target->Set(NanNew("Vertex"), tpl->GetFunction());
}

