#include "BooleanOperation.h"

BooleanOperation::BooleanOperation()
    :m_bop(0)
{
}
BooleanOperation::~BooleanOperation()
{
    delete m_bop;
}
Persistent<FunctionTemplate> BooleanOperation::constructor;


Handle<Object> BooleanOperation::NewInstance(BOP_Operation op)
{
    HandleScope scope;

    Local<Object> instance = constructor->GetFunction()->NewInstance(0,0);

    BooleanOperation* pThis = ObjectWrap::Unwrap<BooleanOperation>(instance);



    return scope.Close(instance);
}

BOP_Operation ReadOperationType(const Handle<Value>& arg)
{
    if (arg->ToString()->Equals(v8::String::NewSymbol("SECTION")))  return BOP_SECTION;
    if (arg->ToString()->Equals(v8::String::NewSymbol("COMMON")))   return BOP_COMMON;
    if (arg->ToString()->Equals(v8::String::NewSymbol("FUSE")))     return BOP_FUSE;
    if (arg->ToString()->Equals(v8::String::NewSymbol("CUT")))      return BOP_CUT;
    if (arg->ToString()->Equals(v8::String::NewSymbol("CUT21")))    return BOP_CUT21;
    return BOP_UNKNOWN;
}

Handle<Value> BooleanOperation::New(const v8::Arguments& args)
{
    HandleScope scope;
    BooleanOperation* pThis = new BooleanOperation();
    pThis->Wrap(args.This());

    BOP_Operation op = ReadOperationType(args[0]);
    if (op == BOP_UNKNOWN) {
        ThrowException(Exception::TypeError(String::New("bad operation type, must be SECTION COMMON FUSE CUT or CUT21")));
        return scope.Close(Undefined());
    }
    return args.This();
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
    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(BooleanOperation::New));
    constructor->SetClassName(String::NewSymbol("BooleanOperation"));

    // object has one internal filed ( the C++ object)
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

    // xx Base::InitProto(proto);
    //xx EXPOSE_READ_ONLY_PROPERTY_INTEGER(Face,numWires);
    //xx EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Face,area);
//XX    EXPOSE_METHOD(BooleanOperation,getSameShape1);
//XX    EXPOSE_METHOD(BooleanOperation,getSameShape2);
//XX    EXPOSE_READ_ONLY_PROPERTY(BooleanOperation,_shape1,shape1);
//XX    EXPOSE_READ_ONLY_PROPERTY(BooleanOperation,_shape2,shape2);

    target->Set(String::NewSymbol("BooleanOperation"), constructor->GetFunction());
}
