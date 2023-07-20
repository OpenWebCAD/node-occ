#include "BooleanOperation.h"
#include "BoundingBox.h"
#include "Edge.h"
#include "Face.h"
#include "GeometryBuilder.h"
#include "Mesh.h"
#include "NodeV8.h"
#include "ShapeFactory.h"
#include "ShapeIterator.h"
#include "Shell.h"
#include "Solid.h"
#include "Tools.h"
#include "Transformation.h"
#include "Util.h"
#include "Vertex.h"
#include "Wire.h"

NAN_METHOD(ForceGC) { Nan::IdleNotification(100); }

NAN_MODULE_INIT(Init) {
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

  Nan::SetMethod(target, "makePlaneMirror", Transformation::makePlaneMirror);

  // Vertex
  Nan::SetMethod(target, "makeVertex", ShapeFactory::makeVertex);

  // edges
  Nan::SetMethod(target, "makeLine", Edge::static_makeLine);
  Nan::SetMethod(target, "makeCircle", Edge::static_makeCircle);
  Nan::SetMethod(target, "makeArc3P", Edge::static_makeArc3P);

  // xx Nan::SetMethod(target,"makeEdge",    ShapeFactory::makeEdge);

  Nan::SetMethod(target, "makeWire", ShapeFactory::makeWire);
  Nan::SetMethod(target, "makeFace", ShapeFactory::makeFace);

  //----------------------------------------------------------
  Nan::SetMethod(target, "makeBox", ShapeFactory::makeBox);
  Nan::SetMethod(target, "makeCylinder", ShapeFactory::makeCylinder);
  Nan::SetMethod(target, "makeCone", ShapeFactory::makeCone);
  Nan::SetMethod(target, "makeSphere", ShapeFactory::makeSphere);
  Nan::SetMethod(target, "makeTorus", ShapeFactory::makeTorus);
  Nan::SetMethod(target, "makePrism", ShapeFactory::makePrism);
  Nan::SetMethod(target, "makeThickSolid", ShapeFactory::makeThickSolid);
  Nan::SetMethod(target, "makeDraftAngle", ShapeFactory::makeDraftAngle);
  Nan::SetMethod(target, "makeFillet", ShapeFactory::makeFillet);
  Nan::SetMethod(target, "makeRevol", ShapeFactory::makeRevol);
  Nan::SetMethod(target, "makePipe", ShapeFactory::makePipe);
  Nan::SetMethod(target, "makePipeShell", ShapeFactory::makePipeShell);
  // Nan::SetMethod(target, "makePipeSolid", ShapeFactory::makePipeSolid);
  Nan::SetMethod(target, "makeSolidThruSections",
                 ShapeFactory::makeSolidThruSections);

  // target->Set(NanSymbol("makeChamfer")NanNew<FunctionTemplate>(ShapeFactory::makeDraftAngle);

  Nan::SetMethod(target, "fuse", ShapeFactory::fuse);
  Nan::SetMethod(target, "cut", ShapeFactory::cut);
  Nan::SetMethod(target, "common", ShapeFactory::common);
  Nan::SetMethod(target, "compound", ShapeFactory::compound);

  Nan::SetMethod(target, "writeSTL", writeSTL);
  Nan::SetMethod(target, "writeSTEP", writeSTEP);
  Nan::SetMethod(target, "writeBREP", writeBREP);
  Nan::SetMethod(target, "writeGLTF", writeGLTF);

  Nan::SetMethod(target, "readSTEP", readSTEP);
  Nan::SetMethod(target, "readBREP", readBREP);

  // xx Nan::SetMethod(target,"oceVersion",NanNew("0.13"));
  Nan::Set(target, Nan::New("oceVersion").ToLocalChecked(),
           Nan::New("0.13").ToLocalChecked());

  Nan::SetMethod(target, "gc", ForceGC);
}
NODE_MODULE(occ, Init)
