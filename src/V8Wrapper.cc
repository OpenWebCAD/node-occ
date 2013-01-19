#include "NodeV8.h"
#include "Util.h"
#include "GeometryBuilder.h"
#include "BoundingBox.h"
#include "Solid.h"
#include "Mesh.h"
#include "Edge.h"
#include "Vertex.h"
#include "Wire.h"
#include "Face.h"
#include "Transformation.h"
#include "ShapeIterator.h"
#include "Tools.h"
#include "ShapeFactory.h"
#include "Shell.h"
#include "BooleanOperation.h"



Handle<Value>  ForceGC(const Arguments& args)
{
    HandleScope scope;
    while(!V8::IdleNotification()) {}
    return scope.Close(Undefined());

}
void Initialize(Handle<Object> target)
{

    BoundingBox::Init(target);
    Edge::Init(target);
    Face::Init(target);
    Mesh::Init(target);
    Point3Wrap::Init(target);
    ShapeIterator::Init(target);
    Shell::Init(target);
    Solid::Init(target);
    Transformation::Init(target);
    Vertex::Init(target);
    Wire::Init(target);
    BooleanOperation::Init(target);

    target->Set(String::NewSymbol("makeBox"),     FunctionTemplate::New(ShapeFactory::makeBox)->GetFunction());
    target->Set(String::NewSymbol("makeCylinder"),FunctionTemplate::New(ShapeFactory::makeCylinder)->GetFunction());
    target->Set(String::NewSymbol("makeCone"),    FunctionTemplate::New(ShapeFactory::makeCone)->GetFunction());
    target->Set(String::NewSymbol("makeSphere"),  FunctionTemplate::New(ShapeFactory::makeSphere)->GetFunction());
    target->Set(String::NewSymbol("makeTorus"),   FunctionTemplate::New(ShapeFactory::makeTorus)->GetFunction());
    target->Set(String::NewSymbol("makePrism"),   FunctionTemplate::New(ShapeFactory::makePrism)->GetFunction());

    target->Set(String::NewSymbol("fuse"),        FunctionTemplate::New(ShapeFactory::fuse)->GetFunction());
    target->Set(String::NewSymbol("cut"),         FunctionTemplate::New(ShapeFactory::cut)->GetFunction());
    target->Set(String::NewSymbol("common"),      FunctionTemplate::New(ShapeFactory::common)->GetFunction());
    target->Set(String::NewSymbol("compound"),    FunctionTemplate::New(ShapeFactory::compound)->GetFunction());

    target->Set(String::NewSymbol("writeSTEP"),   FunctionTemplate::New(writeSTEP)->GetFunction());

    target->Set(String::NewSymbol("oceVersion"),  String::New("0.11"));
    target->Set(String::NewSymbol("gc"),     FunctionTemplate::New(ForceGC)->GetFunction());


}
NODE_MODULE(occ, Initialize)
