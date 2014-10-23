#include "Shell.h"
#include "Face.h"
#include "Wire.h"
#include "Edge.h"
#include "Util.h"


const TopoDS_Shape&  Shell::shape() const
{
    return shell();
}

void Shell::setShape(const TopoDS_Shape& shape)
{
    m_shell = TopoDS::Shell(shape);
}

int Shell::numFaces()
{
    if(shape().IsNull()) return 0;
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(shape(), TopAbs_FACE, anIndices);
    return anIndices.Extent();
}



double Shell::area()
{
    GProp_GProps prop;
    BRepGProp::SurfaceProperties(shape(), prop);
    return prop.Mass();
}


// DVec Face::inertia() {
//    DVec ret;
//    GProp_GProps prop;
//    BRepGProp::SurfaceProperties(this->getShape(), prop);
//    gp_Mat mat = prop.MatrixOfInertia();
//    ret.push_back(mat(1,1)); // Ixx
//    ret.push_back(mat(2,2)); // Iyy
//    ret.push_back(mat(3,3)); // Izz
//    ret.push_back(mat(1,2)); // Ixy
//    ret.push_back(mat(1,3)); // Ixz
//    ret.push_back(mat(2,3)); // Iyz
//    return ret;
//}
//
//OCCStruct3d Face::centreOfMass() {
//    OCCStruct3d ret;
//    GProp_GProps prop;
//    BRepGProp::SurfaceProperties(this->getShape(), prop);
//    gp_Pnt cg = prop.CentreOfMass();
//    ret.x = cg.X();
//    ret.y = cg.Y();
//    ret.z = cg.Z();
//    return ret;
//}
//


Persistent<FunctionTemplate> Shell::_template;


NAN_METHOD(Shell::New)
{
    NanScope();
    Shell* obj = new Shell();
    obj->Wrap(args.This());
    NanReturnValue(args.This());
}

Local<Object>  Shell::Clone() const
{
    Shell* obj = new Shell();
    Local<Object> instance = NanNew(_template)->GetFunction()->NewInstance();
    obj->Wrap(instance);
    obj->setShape(this->shape());
    return instance;
}

void Shell::Init(Handle<Object> target)
{
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(Shell::New);  
    tpl->SetClassName(NanNew("Shell"));

    // object has one internal filed ( the C++ object)
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NanAssignPersistent<v8::FunctionTemplate>(_template, tpl);

    // Prototype
    Local<ObjectTemplate> proto = tpl->PrototypeTemplate();

    Base::InitProto(proto);

    EXPOSE_READ_ONLY_PROPERTY_INTEGER(Shell,numFaces);
    EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Shell,area);

    target->Set(NanNew("Shell"), tpl->GetFunction());
}
