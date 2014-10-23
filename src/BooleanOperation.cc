#include "BooleanOperation.h"

BooleanOperation::BooleanOperation()
  :m_bop(0)
{
}
BooleanOperation::~BooleanOperation()
{
  delete m_bop;
}

v8::Persistent<v8::FunctionTemplate> BooleanOperation::_template;


v8::Handle<v8::Value> BooleanOperation::NewInstance(BOPAlgo_Operation op)
{

  Local<Object> instance = NanNew<v8::FunctionTemplate>(_template)->GetFunction()->NewInstance(0,0);
  BooleanOperation* pThis = ObjectWrap::Unwrap<BooleanOperation>(instance);
  return instance;
}

BOPAlgo_Operation ReadOperationType(const Handle<Value>& arg)
{
  if (arg->ToString()->Equals(NanNew("SECTION")))  return BOPAlgo_SECTION;
  if (arg->ToString()->Equals(NanNew("COMMON")))   return BOPAlgo_COMMON;
  if (arg->ToString()->Equals(NanNew("FUSE")))     return BOPAlgo_FUSE;
  if (arg->ToString()->Equals(NanNew("CUT")))      return BOPAlgo_CUT;
  if (arg->ToString()->Equals(NanNew("CUT21")))    return BOPAlgo_CUT21;
  return BOPAlgo_UNKNOWN;
}

NAN_METHOD(BooleanOperation::New)
{
  NanScope();

  BooleanOperation* pThis = new BooleanOperation();
  pThis->Wrap(args.This());

  BOPAlgo_Operation op = ReadOperationType(args[0]);
  if (op == BOPAlgo_UNKNOWN) {

    NanThrowError("bad operation type, must be SECTION COMMON FUSE CUT or CUT21");
    ///NanThrowError("bad operation type, must be SECTION COMMON FUSE CUT or CUT21");
    NanReturnUndefined();
  }

  NanReturnValue(args.This());
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
BooleanOperation* pThis = node::ObjectWrap::Unwrap<BooleanOperation>(pJhis);


return scope.Close(arr);
}
Handle<Value> BooleanOperation_getSameShape2(const v8::Arguments& args)
{
}
*/

void BooleanOperation::Init(Handle<Object> target)
{

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(BooleanOperation::New);
  tpl->SetClassName(NanNew("BooleanOperation"));

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  
  NanAssignPersistent<v8::FunctionTemplate>(_template, tpl);
  // constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(BooleanOperation::New));


  // Prototype
  //xx Local<ObjectTemplate> proto = constructor->PrototypeTemplate();
  // xx Base::InitProto(proto);
  //xx EXPOSE_READ_ONLY_PROPERTY_INTEGER(Face,numWires);
  //xx EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Face,area);
  //XX    EXPOSE_METHOD(BooleanOperation,getSameShape1);
  //XX    EXPOSE_METHOD(BooleanOperation,getSameShape2);
  //XX    EXPOSE_READ_ONLY_PROPERTY(BooleanOperation,_shape1,shape1);
  //XX    EXPOSE_READ_ONLY_PROPERTY(BooleanOperation,_shape2,shape2);

  target->Set(NanNew("BooleanOperation"), tpl->GetFunction());
}
