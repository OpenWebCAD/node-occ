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



NAN_METHOD(ForceGC)
{
    NanScope();
    while(!V8::IdleNotification()) {}
    NanReturnUndefined();

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

    NODE_SET_METHOD(target,"makeBox",ShapeFactory::makeBox);
    NODE_SET_METHOD(target,"makeBox",ShapeFactory::makeBox);
    NODE_SET_METHOD(target,"makeCylinder",ShapeFactory::makeCylinder);
    NODE_SET_METHOD(target,"makeCone",ShapeFactory::makeCone);
    NODE_SET_METHOD(target,"makeSphere",ShapeFactory::makeSphere);
    NODE_SET_METHOD(target,"makeTorus",ShapeFactory::makeTorus);
    NODE_SET_METHOD(target,"makePrism",ShapeFactory::makePrism);
    NODE_SET_METHOD(target,"makeThickSolid",ShapeFactory::makeThickSolid);
    NODE_SET_METHOD(target,"makeDraftAngle",ShapeFactory::makeDraftAngle);
    NODE_SET_METHOD(target,"makeFillet",ShapeFactory::makeFillet);
   // target->Set(NanSymbol("makeChamfer")NanNew<FunctionTemplate>(ShapeFactory::makeDraftAngle);


    NODE_SET_METHOD(target,"fuse",ShapeFactory::fuse);
    NODE_SET_METHOD(target,"cut",ShapeFactory::cut);
    NODE_SET_METHOD(target,"common",ShapeFactory::common);
    NODE_SET_METHOD(target,"compound",ShapeFactory::compound);

    NODE_SET_METHOD(target,"writeSTL",writeSTL);
    NODE_SET_METHOD(target,"writeSTEP",writeSTEP);
    NODE_SET_METHOD(target,"writeBREP",writeBREP);
    NODE_SET_METHOD(target,"readSTEP",readSTEP);
    NODE_SET_METHOD(target,"readBREP",readBREP);

    //xx NODE_SET_METHOD(target,"oceVersion",NanNew("0.13"));
    target->Set(NanNew("oceVersion"),  NanNew("0.13"));

    NODE_SET_METHOD(target,"gc",ForceGC);
#if 0
    target->Set(NanNew("makeBox"),      NanNew<FunctionTemplate>(ShapeFactory::makeBox)->GetFunction());
    target->Set(NanNew("makeCylinder"), NanNew<FunctionTemplate>(ShapeFactory::makeCylinder)->GetFunction());
    target->Set(NanNew("makeCone"),         NanNew<FunctionTemplate>(ShapeFactory::makeCone)->GetFunction());
    target->Set(NanNew("makeSphere"),       NanNew<FunctionTemplate>(ShapeFactory::makeSphere)->GetFunction());
    target->Set(NanNew("makeTorus"),        NanNew<FunctionTemplate>(ShapeFactory::makeTorus)->GetFunction());
    target->Set(NanNew("makePrism"),        NanNew<FunctionTemplate>(ShapeFactory::makePrism)->GetFunction());
    target->Set(NanNew("makeThickSolid"),   NanNew<FunctionTemplate>(ShapeFactory::makeThickSolid)->GetFunction());
    target->Set(NanNew("makeDraftAngle"),   NanNew<FunctionTemplate>(ShapeFactory::makeDraftAngle)->GetFunction());
    target->Set(NanNew("makeFillet"),   NanNew<FunctionTemplate>(ShapeFactory::makeFillet)->GetFunction());
   // target->Set(NanSymbol("makeChamfer")  NanNew<FunctionTemplate>(ShapeFactory::makeDraftAngle)->GetFunction());


    target->Set(NanNew("fuse"),        NanNew<FunctionTemplate>(ShapeFactory::fuse)->GetFunction());
    target->Set(NanNew("cut"),         NanNew<FunctionTemplate>(ShapeFactory::cut)->GetFunction());
    target->Set(NanNew("common"),      NanNew<FunctionTemplate>(ShapeFactory::common)->GetFunction());
    target->Set(NanNew("compound"),    NanNew<FunctionTemplate>(ShapeFactory::compound)->GetFunction());

    target->Set(NanNew("writeSTL"),   NanNew<FunctionTemplate>(writeSTL)->GetFunction());
    target->Set(NanNew("writeSTEP"),   NanNew<FunctionTemplate>(writeSTEP)->GetFunction());
    target->Set(NanNew("writeBREP"),   NanNew<FunctionTemplate>(writeBREP)->GetFunction());
    target->Set(NanNew("readSTEP"),   NanNew<FunctionTemplate>(readSTEP)->GetFunction());
    target->Set(NanNew("readBREP"),   NanNew<FunctionTemplate>(readBREP)->GetFunction());

    target->Set(NanNew("oceVersion"),  NanNew("0.13"));
    target->Set(NanNew("gc"),     NanNew<FunctionTemplate>(ForceGC)->GetFunction());
#endif

}
NODE_MODULE(occ, Initialize)
