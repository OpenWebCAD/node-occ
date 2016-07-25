#include "BoundingBox.h"

#include "Util.h"
#include <limits.h>

Nan::Persistent<v8::FunctionTemplate> BoundingBox::_template;


v8::Handle<v8::Value> BoundingBox::NewInstance(const Bnd_Box& box)
{

  v8::Local<v8::Object> instance = Nan::New<v8::FunctionTemplate>(_template)->GetFunction()->NewInstance(Nan::GetCurrentContext(), 0, 0).ToLocalChecked();

  BoundingBox* pThis = ObjectWrap::Unwrap<BoundingBox>(instance);

  pThis->m_box = box;

  return instance;
}

v8::Handle<v8::Value> BoundingBox::NewInstance(const gp_Pnt& nearPt,const gp_Pnt& farPt)
{
  Bnd_Box box;
  box.Add(nearPt);
  box.Add(farPt);
  return NewInstance(box);
}

void BoundingBox::Update(BoundingBox* pThis,_NAN_METHOD_ARGS)
{
  // info could be one or several points
  // or a array of point
  for (int i=0; i<info.Length(); i++) {
    if (info[i]->IsArray()) {
      v8::Handle<v8::Array> arr = v8::Handle<v8::Array>::Cast(info[i]);
      if ( arr->Get(0)->IsArray() || arr->Get(0)->IsObject()) {
        // probably an array of point
      } else {
        // a single point
        gp_Pnt point;
        ReadPoint(info[i],&point);
        pThis->m_box.Update(point.X(),point.Y(),point.Z());
      }
    }
  }
}


NAN_METHOD(BoundingBox::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowError(" use new occ.BoundingBox() to construct a BoundingBox");
  }

  BoundingBox* pThis = new BoundingBox();
  pThis->Wrap(info.This());

  BoundingBox::Update(pThis,info);
  pThis->InitNew(info);

  info.GetReturnValue().Set(info.This());

}

NAN_METHOD(BoundingBox::addPoint)
{

  BoundingBox* pThis = ObjectWrap::Unwrap<BoundingBox>(info.This());
  BoundingBox::Update(pThis,info);

  info.GetReturnValue().Set(info.This());
}

bool checkCoerceToPoint(const v8::Handle<v8::Value>& v)
{ 
  // TODO ...
  return true;
}

NAN_METHOD(BoundingBox::isOut)
{

  BoundingBox* pThis = ObjectWrap::Unwrap<BoundingBox>(info.This());
  bool _itOut = false;

  if (info.Length() != 1 || !checkCoerceToPoint(info[0])) {
    return Nan::ThrowError(" error expecting a point or a arrya of 3 doubles");
  }
  gp_Pnt point;
  ReadPoint(info[0],&point);

  bool retVal = pThis->m_box.IsOut(point)?true:false;

  info.GetReturnValue().Set(Nan::New<v8::Boolean>(retVal));
}


void BoundingBox::Init(v8::Handle<v8::Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(BoundingBox::New);
  tpl->SetClassName(Nan::New("BoundingBox").ToLocalChecked());

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  EXPOSE_METHOD(BoundingBox,addPoint);
  EXPOSE_METHOD(BoundingBox,isOut);

  EXPOSE_TEAROFF(BoundingBox,nearPt);
  EXPOSE_TEAROFF(BoundingBox,farPt);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(BoundingBox,isVoid);

  target->Set(Nan::New("BoundingBox").ToLocalChecked(), tpl->GetFunction());
}

void BoundingBox::InitNew(_NAN_METHOD_ARGS)
{
  //xx Base::InitNew(info);
  REXPOSE_TEAROFF(BoundingBox,nearPt);
  REXPOSE_TEAROFF(BoundingBox,farPt);
}