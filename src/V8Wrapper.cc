// #define BUILDING_NODE_EXTENSION
#include <node.h>
#include <v8.h>

using namespace v8;

void ReadPropertyPointFromArray( Local<Value>& value,double* x,double* y, double*z ) 
{
    Local<Object> obj = value->ToObject();

    Handle<Array> array = Handle<Array>::Cast(obj);
    int length = array->Length();
    // int length = obj->Get(String::New("length"))->ToObject()->Uint32Value();

     if (length>=1) {  *x = obj->Get(0)->NumberValue();  }
     if (length>=2) {  *y = obj->Get(1)->NumberValue();  }
     if (length>=3) {  *z = obj->Get(2)->NumberValue();  }
  
}

void ReadPropertyPoint( Local<Value>& value,const char* name,double* x,double* y, double*z ) 
{
  if (value->IsObject()) {
     Local<Object> obj = value->ToObject();
    // for exemple a THREE.Vector3 
    // ( we try to read the "x","y","z" property )
    return ReadPropertyPointFromArray(obj->Get(String::New(name)),x,y,z);
  }


  if (value->IsArray()) {
    ReadPropertyPointFromArray(value,x,y,z);
  }
}

inline Handle<Value> Cuboid(const Arguments& args)
{

  HandleScope scope;
  
  if (args.Length() < 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }
  
  double x=0;double y=0;double z=0;
  ReadPropertyPoint(args[0],"position",&x,&y,&z);

  // resulting object
  Local<Object> obj = Object::New();
  obj->Set(String::NewSymbol("x"), Number::New(x));
  obj->Set(String::NewSymbol("y"), Number::New(y));
  obj->Set(String::NewSymbol("z"), Number::New(z));

  return scope.Close(obj);
}

Handle<Value> Hello(const Arguments& args) {
  HandleScope scope;
  return scope.Close(String::New("world"));
}



Handle<Value> Add(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    return scope.Close(Undefined());
  }

  Local<Number> num = Number::New(args[0]->NumberValue() +
      args[1]->NumberValue());
  return scope.Close(num);
}


void Initialize(Handle<Object> target) {
  
  NODE_SET_METHOD(target, "Cuboid", Cuboid);
  
}
NODE_MODULE(binding, Initialize)


