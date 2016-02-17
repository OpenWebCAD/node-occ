#include "Transformation.h"
#include "Util.h"


NAN_METHOD(Transformation::makeTranslation)
{
  Transformation* pThis = Transformation::Unwrap<Transformation>(info.This());
  if( info.Length()!=1) {
    return Nan::ThrowError("Wrong arguments");
  }
  double x=0,y=0,z=0;
  ReadPoint(info[0],&x,&y,&z);

  pThis->m_trsf.SetTranslation(gp_Vec(x,y,z));
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Transformation::makePlaneMirror)
{
  Transformation* pThis = Transformation::Unwrap<Transformation>(info.This());
  if(info.Length()!=2) {
    return Nan::ThrowError("Wrong arguments");
  }

  double x=0,y=0,z=0;
  ReadPoint(info[0],&x,&y,&z);

  double u=0,v=0,w=0;
  ReadPoint(info[1],&u,&v,&w);

  Standard_Real D = sqrt (u * u + v * v + w * w);
  if (D <= gp::Resolution()) {
    return Nan::ThrowError("Plane Axis direction is null");
  }

  pThis->m_trsf.SetMirror(gp_Ax2(gp_Pnt(x, y, z), gp_Dir(u, v, w)));
  info.GetReturnValue().Set(info.This());
}


NAN_METHOD(Transformation::makeAxisMirror)
{
  Transformation* pThis = Transformation::Unwrap<Transformation>(info.This());
  if(info.Length()!=2) {
    return Nan::ThrowError("Wrong arguments");
  }

  double x=0,y=0,z=0;
  ReadPoint(info[0],&x,&y,&z);

  double u=0,v=0,w=0;
  ReadPoint(info[1],&u,&v,&w);

  Standard_Real D = sqrt (u * u + v * v + w * w);
  if (D <= gp::Resolution()) {
    return Nan::ThrowError("Lirror Axis direction is null");
  }

  pThis->m_trsf.SetMirror(gp_Ax1(gp_Pnt(x, y, z), gp_Dir(u, v, w)));

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Transformation::makeScale)
{
  Transformation* pThis = Transformation::Unwrap<Transformation>(info.This());

  if(info.Length()!=2) {
    return Nan::ThrowError("Wrong arguments");
  }

  double factor= info[0]->NumberValue();

  double x=0,y=0,z=0;
  ReadPoint(info[1],&x,&y,&z);

  pThis->m_trsf.SetScale(gp_Pnt(x, y, z), factor);

  info.GetReturnValue().Set(info.This());
}



NAN_METHOD(Transformation::makeRotation)
{
  Transformation* pThis = Transformation::Unwrap<Transformation>(info.This());

  if(info.Length()!=3) {
    return Nan::ThrowError("Wrong arguments in makeRotation");
  }

  ReadRotationFromArgs(info, pThis->m_trsf);

  info.GetReturnValue().Set(info.This());
}

// Methods exposed to JavaScripts
Nan::Persistent<v8::FunctionTemplate> Transformation::_template;

void Transformation::Init(v8::Handle<v8::Object> target)
{
  v8::Local<v8::FunctionTemplate> tpl =  Nan::New<v8::FunctionTemplate>(Transformation::New);
  // Prepare constructor template
  tpl->SetClassName(Nan::New("Transformation").ToLocalChecked());

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  //xx NanAssignPersistent<v8::FunctionTemplate>(_template,tpl);
  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  EXPOSE_METHOD(Transformation,makeRotation);
  EXPOSE_METHOD(Transformation,makeTranslation);
  EXPOSE_METHOD(Transformation,makePlaneMirror);
  EXPOSE_METHOD(Transformation,makeAxisMirror);
  EXPOSE_METHOD(Transformation,makeScale);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Transformation,scaleFactor);

  target->Set(Nan::New("Transformation").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(Transformation::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowError(" use new occ.Transformation() to construct a transformation");
  }

  Transformation* obj = new Transformation();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Transformation::NewInstance)
{
  v8::Local<v8::Object> instance = Nan::New<v8::FunctionTemplate>(_template)->GetFunction()->NewInstance(0,0);
  info.GetReturnValue().Set(instance);
}