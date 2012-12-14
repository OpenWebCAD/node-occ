#include "NodeV8.h"
#include "Util.h"
#include "GeometryBuilder.h"
#include "Solid.h"
#include "Transformation.h"


v8::Handle<v8::Value> createBox(const v8::Arguments& args)
{
  v8::HandleScope scope;
  
  if (args.Length() < 1) {
    v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong number of arguments")));
    return scope.Close(v8::Undefined());
  }
  
  // resulting object
  v8::Handle<v8::Value> obj = Solid::NewInstance(args);  

  return scope.Close(Solid::makeBox(args));
}

//v8::Handle<v8::Value> Hello(const v8::Arguments& args)
//{
//  v8::HandleScope scope;
//  return scope.Close(v8::String::New("world"));
//}
//
//
//
//v8::Handle<v8::Value> Add(const v8::Arguments& args)
//{
//  v8::HandleScope scope;
//
//  if (args.Length() < 2) {
//    v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong number of arguments")));
//    return scope.Close(v8::Undefined());
//  }
//
//  if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
//    v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
//    return scope.Close(v8::Undefined());
//  }
//
//  v8::Local<v8::Number> num = v8::Number::New(args[0]->NumberValue() +
//      args[1]->NumberValue());
//  return scope.Close(num);
//}


void Initialize(v8::Handle<v8::Object> target) 
{
  
  Solid::Init(target);
  Transformation::Init(target);

  NODE_SET_METHOD(target, "createBox", createBox);
}
NODE_MODULE(occ, Initialize)


