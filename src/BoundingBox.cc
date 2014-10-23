#include "BoundingBox.h"

#include "Util.h"
#include <limits.h>

v8::Persistent<v8::FunctionTemplate> BoundingBox::_template;


v8::Handle<v8::Value> BoundingBox::NewInstance(const Bnd_Box& box)
{

  Local<Object> instance = NanNew<v8::FunctionTemplate>(_template)->GetFunction()->NewInstance(0,0);

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
  // args could be one or several points
  // or a array of point
  for (int i=0; i<args.Length(); i++) {
    if (args[i]->IsArray()) {
      Handle<Array> arr = Handle<Array>::Cast(args[i]);
      if ( arr->Get(0)->IsArray() || arr->Get(0)->IsObject()) {
        // probably an array of point
      } else {
        // a single point
        gp_Pnt point;
        ReadPoint(args[i],&point);
        pThis->m_box.Update(point.X(),point.Y(),point.Z());
      }
    }
  }
}


NAN_METHOD(BoundingBox::New)
{
  NanScope();
  
  BoundingBox* pThis = new BoundingBox();
  pThis->Wrap(args.This());
  
  BoundingBox::Update(pThis,args);

  NanReturnValue(args.This());

}

NAN_METHOD(BoundingBox::addPoint)
{
  NanScope();

  BoundingBox* pThis = ObjectWrap::Unwrap<BoundingBox>(args.This());
  BoundingBox::Update(pThis,args);

  NanReturnValue(args.This());
}

bool checkCoerceToPoint(const Handle<Value>& v) 
{ 
  // TODO ...
  return true;
}

NAN_METHOD(BoundingBox::isOut)
{
  NanScope();

  BoundingBox* pThis = ObjectWrap::Unwrap<BoundingBox>(args.This());
  bool _itOut = false;

  if (args.Length() != 1 || !checkCoerceToPoint(args[0])) {

    NanThrowError(" error expecting a point or a arrya of 3 doubles");
    NanReturnUndefined();
  }
  gp_Pnt point;
  ReadPoint(args[0],&point);

  bool retVal = pThis->m_box.IsOut(point)?true:false;

  NanReturnValue(NanNew<Boolean>(retVal));
}


void BoundingBox::Init(Handle<Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(BoundingBox::New);
  tpl->SetClassName(NanNew("BoundingBox"));

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NanAssignPersistent<v8::FunctionTemplate>(_template, tpl);

  // Prototype
  Local<ObjectTemplate> proto = tpl->PrototypeTemplate();

  EXPOSE_METHOD(BoundingBox,addPoint);
  EXPOSE_METHOD(BoundingBox,isOut);

  EXPOSE_TEAROFF(BoundingBox,nearPt);
  EXPOSE_TEAROFF(BoundingBox,farPt);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(BoundingBox,isVoid);


  target->Set(NanNew("BoundingBox"), tpl->GetFunction());
}