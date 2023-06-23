#include "Shell.h"
#include "Edge.h"
#include "Face.h"
#include "Util.h"
#include "Wire.h"

const TopoDS_Shape &Shell::shape() const { return shell(); }

void Shell::setShape(const TopoDS_Shape &shape) {
  m_shell = TopoDS::Shell(shape);
}

int Shell::numFaces() {
  if (shape().IsNull())
    return 0;
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(shape(), TopAbs_FACE, anIndices);
  return anIndices.Extent();
}

double Shell::area() {
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
// OCCStruct3d Face::centreOfMass() {
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

Nan::Persistent<v8::FunctionTemplate> Shell::_template;

NAN_METHOD(Shell::New) {
  if (!info.IsConstructCall()) {
    return Nan::ThrowError(" use new occ.Shell() to construct a Shell");
  }
  Shell *pThis = new Shell();
  pThis->Wrap(info.This());
  pThis->InitNew(info);

  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Object> Shell::Clone() const {
  Nan::EscapableHandleScope scope;

  v8::Local<v8::Object> instance = makeInstance(_template);
  Shell *obj = new Shell();
  obj->Wrap(instance);
  obj->setShape(this->shape());
  return scope.Escape(instance);
}

NAN_MODULE_INIT(Shell::Init) {
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl =
      Nan::New<v8::FunctionTemplate>(Shell::New);
  tpl->SetClassName(Nan::New("Shell").ToLocalChecked());

  // object has one internal field ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  Base::InitProto(proto);

  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Shell, numFaces);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Shell, area);

  Nan::Set(target, Nan::New("Shell").ToLocalChecked(),
           Nan::GetFunction(tpl).ToLocalChecked());
}
