#include "BooleanOperation.h"

BooleanOperation::BooleanOperation()
  :m_bop(0)
{
}
BooleanOperation::~BooleanOperation()
{
  delete m_bop;
}

Nan::Persistent<v8::FunctionTemplate> BooleanOperation::_template;


v8::Local<v8::Value> BooleanOperation::NewInstance(BOPAlgo_Operation op)
{

  v8::Local<v8::Object> instance = makeInstance(_template);
  BooleanOperation* pThis = ObjectWrap::Unwrap<BooleanOperation>(instance);
  return instance;
}


inline bool _isEqual(v8::Local<v8::String> a,const std::string txt) {
  return txt == *Nan::Utf8String(a);
}

BOPAlgo_Operation ReadOperationType(const v8::Local<v8::Value>& arg)
{

  v8::Local<v8::String> str = Nan::To<v8::String>(arg).ToLocalChecked();
  if (_isEqual(str,"SECTION"))  return BOPAlgo_SECTION;
  if (_isEqual(str,"COMMON"))   return BOPAlgo_COMMON;
  if (_isEqual(str,"FUSE"))     return BOPAlgo_FUSE;
  if (_isEqual(str,"CUT"))      return BOPAlgo_CUT;
  if (_isEqual(str,"CUT21"))    return BOPAlgo_CUT21;
  return BOPAlgo_UNKNOWN;
}

NAN_METHOD(BooleanOperation::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowError(" use new occ.BooleanOperation() to construct a BooleanOperation");
  }

  BooleanOperation* pThis = new BooleanOperation();
  pThis->Wrap(info.This());

  BOPAlgo_Operation op = ReadOperationType(info[0]);
  if (op == BOPAlgo_UNKNOWN) {
    return Nan::ThrowError("bad operation type, must be SECTION COMMON FUSE CUT or CUT21");
  }

  info.GetReturnValue().Set(info.This());
}

/*
Handle<Value> BooleanOperation::getSameShape1(const v8::Arguments& args)
{

HandleScope scope;
// can work with this
Handle<Object> pJhis = args.This();
if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
// create a new object
ThrowException(Exception::Error(String::New("invalid object")));
}
BooleanOperation* pThis = Nan::ObjectWrap::Unwrap<BooleanOperation>(pJhis);


return scope.Close(arr);
}
Handle<Value> BooleanOperation_getSameShape2(const v8::Arguments& args)
{
}
*/

void BooleanOperation::Init(v8::Local<v8::Object> target)
{

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(BooleanOperation::New);
  tpl->SetClassName(Nan::New("BooleanOperation").ToLocalChecked());

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  _template.Reset(tpl);


  // Prototype
  //xx Local<ObjectTemplate> proto = constructor->PrototypeTemplate();
  // xx Base::InitProto(proto);
  //xx EXPOSE_READ_ONLY_PROPERTY_INTEGER(Face,numWires);
  //xx EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Face,area);
  //XX    EXPOSE_METHOD(BooleanOperation,getSameShape1);
  //XX    EXPOSE_METHOD(BooleanOperation,getSameShape2);
  //XX    EXPOSE_READ_ONLY_PROPERTY(BooleanOperation,_shape1,shape1);
  //XX    EXPOSE_READ_ONLY_PROPERTY(BooleanOperation,_shape2,shape2);

  Nan::Set(target, Nan::New("BooleanOperation").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}
