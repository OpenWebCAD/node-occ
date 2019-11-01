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
    Nan::IdleNotification(100);
}

void Initialize(v8::Local<v8::Object> target)
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


    Nan::SetMethod(target,"makePlaneMirror",  Transformation::makePlaneMirror);

    // Vertex
    Nan::SetMethod(target,"makeVertex",        ShapeFactory::makeVertex);


    // edges
    Nan::SetMethod(target,"makeLine",        Edge::static_createLine);
    Nan::SetMethod(target,"makeCircle",      Edge::static_createCircle);
    Nan::SetMethod(target,"makeArc3P",       Edge::static_createArc3P);

    //xx Nan::SetMethod(target,"makeEdge",    ShapeFactory::makeEdge);

    Nan::SetMethod(target,"makeWire",    ShapeFactory::makeWire);
    Nan::SetMethod(target,"makeFace",    ShapeFactory::makeFace);

    //----------------------------------------------------------
    Nan::SetMethod(target,"makeBox",       ShapeFactory::makeBox);
    Nan::SetMethod(target,"makeCylinder",  ShapeFactory::makeCylinder);
    Nan::SetMethod(target,"makeCone",      ShapeFactory::makeCone);
    Nan::SetMethod(target,"makeSphere",    ShapeFactory::makeSphere);
    Nan::SetMethod(target,"makeTorus",     ShapeFactory::makeTorus);
    Nan::SetMethod(target,"makePrism",     ShapeFactory::makePrism);
    Nan::SetMethod(target,"makeThickSolid",ShapeFactory::makeThickSolid);
    Nan::SetMethod(target,"makeDraftAngle",ShapeFactory::makeDraftAngle);
    Nan::SetMethod(target,"makeFillet",    ShapeFactory::makeFillet);
    // target->Set(NanSymbol("makeChamfer")NanNew<FunctionTemplate>(ShapeFactory::makeDraftAngle);

    Nan::SetMethod(target,"fuse",          ShapeFactory::fuse);
    Nan::SetMethod(target,"cut",           ShapeFactory::cut);
    Nan::SetMethod(target,"common",        ShapeFactory::common);
    Nan::SetMethod(target,"compound",      ShapeFactory::compound);

    Nan::SetMethod(target,"writeSTL",      writeSTL);
    Nan::SetMethod(target,"writeSTEP",     writeSTEP);
    Nan::SetMethod(target,"writeBREP",     writeBREP);
    Nan::SetMethod(target,"readSTEP",      readSTEP);
    Nan::SetMethod(target,"readBREP",      readBREP);

    //xx Nan::SetMethod(target,"oceVersion",NanNew("0.13"));
    Nan::Set(target,Nan::New("oceVersion").ToLocalChecked(),  Nan::New("0.13").ToLocalChecked());

    Nan::SetMethod(target,"gc",ForceGC); 


}
NODE_MODULE(occ, Initialize)
