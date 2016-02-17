#include "Point3Wrap.h"
#include "Util.h"

Nan::Persistent<v8::FunctionTemplate> Point3Wrap::_template;

class Point3Wrap1 : public Point3Wrap
{
  gp_XYZ _p;
public:
  Point3Wrap1(double x, double y, double z) :_p(x, y, z) {};
  virtual const gp_XYZ get() const {
     return _p;
  }
};
// Methods exposed to JavaScripts
NAN_METHOD(Point3Wrap::New)
{
  double x = info[0]->ToNumber()->Value();
  double y = info[1]->ToNumber()->Value();
  double z = info[2]->ToNumber()->Value();

  Point3Wrap* obj = new Point3Wrap1(x,y,z);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Point3Wrap::asArray)
{
  Point3Wrap* pThis = node::ObjectWrap::Unwrap<Point3Wrap>(info.This());
  v8::Local<v8::Array> arr = Nan::New<v8::Array>(3);
  arr->Set(0, Nan::New(pThis->x()));
  arr->Set(1, Nan::New(pThis->y()));
  arr->Set(2, Nan::New(pThis->z()));
  info.GetReturnValue().Set(arr);
}
NAN_METHOD(Point3Wrap::equals)
{
  Point3Wrap* pThis = node::ObjectWrap::Unwrap<Point3Wrap>(info.This());
  if (info.Length() == 1) {
    gp_Pnt p1;
    ReadPoint(info[0], &p1);
    bool isEqual = gp_Pnt(pThis->get()).IsEqual(p1, 0.0000001);
    return info.GetReturnValue().Set(isEqual);
  }
  else if (info.Length() == 2) {
    gp_Pnt p1;
    ReadPoint(info[0], &p1);
    double tol = info[1]->ToNumber()->Value();
    bool isEqual = gp_Pnt(pThis->get()).IsEqual(p1, tol);
    return info.GetReturnValue().Set(isEqual);
  }
  Nan::ThrowError("Invalid");
}

// Methods exposed to JavaScripts
void Point3Wrap::Init(v8::Handle<v8::Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(Point3Wrap::New);
  tpl->SetClassName(Nan::New("Point3D").ToLocalChecked());

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Point3Wrap, x);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Point3Wrap, y);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Point3Wrap, z);

  EXPOSE_METHOD(Point3Wrap, equals);
  EXPOSE_METHOD(Point3Wrap, asArray);
  

  target->Set(Nan::New("Point3D").ToLocalChecked(), tpl->GetFunction());

}
