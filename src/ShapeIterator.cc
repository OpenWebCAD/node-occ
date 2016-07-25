#include "ShapeIterator.h"
#include "Solid.h"
#include "Face.h"
#include "Wire.h"
#include "Edge.h"
#include "Vertex.h"
#include "Shell.h"


v8::Local<v8::Object> buildEmptyWrapper(TopAbs_ShapeEnum type)
{
  switch (type) {
  case  TopAbs_COMPOUND:
  case  TopAbs_COMPSOLID:
  case  TopAbs_SOLID:
    return Nan::New(Solid::_template)->GetFunction()->NewInstance(Nan::GetCurrentContext(),0, 0).ToLocalChecked()->ToObject();
  case TopAbs_SHELL:
    return Nan::New(Shell::_template)->GetFunction()->NewInstance(Nan::GetCurrentContext(), 0, 0).ToLocalChecked()->ToObject();
    break;
  case TopAbs_FACE:
    return Nan::New(Face::_template)->GetFunction()->NewInstance(Nan::GetCurrentContext(), 0, 0).ToLocalChecked()->ToObject();
  case TopAbs_WIRE:
    return Nan::New(Wire::_template)->GetFunction()->NewInstance(Nan::GetCurrentContext(), 0, 0).ToLocalChecked()->ToObject();
  case TopAbs_EDGE:
    return Nan::New(Edge::_template)->GetFunction()->NewInstance(Nan::GetCurrentContext(), 0, 0).ToLocalChecked()->ToObject();
  case TopAbs_VERTEX:
    return Nan::New(Vertex::_template)->GetFunction()->NewInstance(Nan::GetCurrentContext(), 0, 0).ToLocalChecked()->ToObject();
  case TopAbs_SHAPE:
    break;
  }
  return v8::Local<v8::Object>();
}
v8::Local<v8::Object> buildWrapper(const TopoDS_Shape shape)
{
  v8::Local<v8::Object> obj = v8::Local<v8::Object>(buildEmptyWrapper(shape.ShapeType()));
  Base*  pShape = Nan::ObjectWrap::Unwrap<Base>(obj);
  pShape->setShape(shape);
  return obj;
}

bool ShapeIterator::more()
{
  return ex.More() ? true : false;
}

v8::Handle<v8::Value> ShapeIterator::next()
{
  if (ex.More()) {

    v8::Local<v8::Object>  obj = buildWrapper(ex.Current());
    Nan::Set(this->handle(), Nan::New("current").ToLocalChecked(), obj);
    ex.Next();
    return obj;
  }
  else {
    return Nan::Undefined();
  }
}

NAN_METHOD(ShapeIterator::next)
{
  ShapeIterator* pThis = UNWRAP(ShapeIterator);
  info.GetReturnValue().Set(pThis->next());
}


void ShapeIterator::reset()
{
  ex.ReInit();
}


NAN_METHOD(ShapeIterator::reset)
{
  ShapeIterator* pThis = UNWRAP(ShapeIterator);
  pThis->reset();
  info.GetReturnValue().Set(info.This());
}

Nan::Persistent<v8::FunctionTemplate> ShapeIterator::_template;

void ShapeIterator::Init(v8::Handle<v8::Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(ShapeIterator::New);
  tpl->SetClassName(Nan::New("ShapeIterator").ToLocalChecked());

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  Base::InitProto(proto);

  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(ShapeIterator, more);

  EXPOSE_METHOD(ShapeIterator, next);
  EXPOSE_METHOD(ShapeIterator, reset);

  target->Set(Nan::New("ShapeIterator").ToLocalChecked(), tpl->GetFunction());

}


TopAbs_ShapeEnum getShapeEnum(const v8::Handle<v8::Value> arg)
{
  if (arg->IsString()) {

    v8::Handle<v8::String> str = arg->ToString();
    if (str->Equals(Nan::New("COMPOUND").ToLocalChecked())) {
      return TopAbs_COMPOUND;
    }
    if (str->Equals(Nan::New("COMPSOLID").ToLocalChecked())) {
      return TopAbs_COMPSOLID;
    }
    if (str->Equals(Nan::New("VERTEX").ToLocalChecked())) {
      return TopAbs_VERTEX;
    }
    if (str->Equals(Nan::New("EDGE").ToLocalChecked())) {
      return TopAbs_EDGE;
    }
    if (str->Equals(Nan::New("WIRE").ToLocalChecked())) {
      return TopAbs_WIRE;
    }
    if (str->Equals(Nan::New("FACE").ToLocalChecked())) {
      return TopAbs_FACE;
    }
    if (str->Equals(Nan::New("SHELL").ToLocalChecked())) {
      return TopAbs_SHELL;
    }
    if (str->Equals(Nan::New("SOLID").ToLocalChecked())) {
      return TopAbs_SOLID;
    }
  }
  return TopAbs_SHAPE;
};

NAN_METHOD(ShapeIterator::New)
{

  if (!info.IsConstructCall()) {
    return Nan::ThrowError(" use new occ.ShapeIterator() to construct a ShapeIterator");
  }

  if (info.Length() != 2) {
    return Nan::ThrowError(" expecting two arguments : <Shape>,<'VERTEX'|'WIRE'|'SOLID'|'FACE'...>");
  }

  // TODO (check that the object info[0] has the correct type)
  Base* pShape = Nan::ObjectWrap::Unwrap<Base>(info[0]->ToObject());

  TopAbs_ShapeEnum type = getShapeEnum(info[1]);

  ShapeIterator* pThis = new ShapeIterator(pShape, type);

  info.This()->Set(Nan::New("current").ToLocalChecked(), Nan::Undefined());

  pThis->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

