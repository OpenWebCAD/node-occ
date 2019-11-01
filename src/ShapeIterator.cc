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
    return makeInstance(Solid::_template);
  case TopAbs_SHELL:
    return makeInstance(Shell::_template);
    break;
  case TopAbs_FACE:
    return makeInstance(Face::_template);
  case TopAbs_WIRE:
    return makeInstance(Wire::_template);
  case TopAbs_EDGE:
    return makeInstance(Edge::_template);
  case TopAbs_VERTEX:
    return makeInstance(Vertex::_template);
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

v8::Local<v8::Value> ShapeIterator::next()
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

void ShapeIterator::Init(v8::Local<v8::Object> target)
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

  Nan::Set(target,Nan::New("ShapeIterator").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());

}


TopAbs_ShapeEnum getShapeEnum(const v8::Local<v8::Value> arg)
{
  if (arg->IsString()) {

    v8::Local<v8::String> str = Nan::To<v8::String>(arg).ToLocalChecked();

    if (Nan::Equals(str, Nan::New("COMPOUND").ToLocalChecked()).ToChecked()) {
      return TopAbs_COMPOUND;
    }
    if (Nan::Equals(str, Nan::New("COMPSOLID").ToLocalChecked()).ToChecked()) {
      return TopAbs_COMPSOLID;
    }
    if (Nan::Equals(str, Nan::New("VERTEX").ToLocalChecked()).ToChecked()) {
      return TopAbs_VERTEX;
    }
    if (Nan::Equals(str, Nan::New("EDGE").ToLocalChecked()).ToChecked()) {
      return TopAbs_EDGE;
    }
    if (Nan::Equals(str, Nan::New("WIRE").ToLocalChecked()).ToChecked()) {
      return TopAbs_WIRE;
    }
    if (Nan::Equals(str, Nan::New("FACE").ToLocalChecked()).ToChecked()) {
      return TopAbs_FACE;
    }
    if (Nan::Equals(str, Nan::New("SHELL").ToLocalChecked()).ToChecked()) {
      return TopAbs_SHELL;
    }
    if (Nan::Equals(str, Nan::New("SOLID").ToLocalChecked()).ToChecked()) {
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
  Base* pShape = Nan::ObjectWrap::Unwrap<Base>(Nan::To<v8::Object>(info[0]).ToLocalChecked());

  TopAbs_ShapeEnum type = getShapeEnum(info[1]);

  ShapeIterator* pThis = new ShapeIterator(pShape, type);

  Nan::Set(info.This(),Nan::New("current").ToLocalChecked(), Nan::Undefined());

  pThis->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

