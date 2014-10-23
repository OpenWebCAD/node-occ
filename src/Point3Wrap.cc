#include "Point3Wrap.h"

Persistent<FunctionTemplate> Point3Wrap::_template;

// Methods exposed to JavaScripts
NAN_METHOD(Point3Wrap::New)
{
  NanScope();
  NanReturnValue(args.This());
}
// Methods exposed to JavaScripts
void Point3Wrap::Init(Handle<Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(Point3Wrap::New);  
  tpl->SetClassName(NanNew("Point3D"));

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NanAssignPersistent<v8::FunctionTemplate>(_template, tpl);

  // Prototype
  Local<ObjectTemplate> proto = tpl->PrototypeTemplate();
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Point3Wrap,x);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Point3Wrap,y);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Point3Wrap,z);

  target->Set(NanNew("Point3D"), tpl->GetFunction());

}
