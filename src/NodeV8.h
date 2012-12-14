// NodeV8
#pragma once
#ifdef _MSC_VER
#pragma warning(disable:4506)
#pragma warning(disable:4355) // warning C4355: 'this' : used in base member initializer list

#endif
#include <node.h>
#include <v8.h>


template<class T,typename T1,typename T2,typename T2 (T::*func)() > inline 
v8::Handle<v8::Value> ee(v8::Local<v8::String> property,const v8::AccessorInfo &info)
{
  T* obj = node::ObjectWrap::Unwrap<T>(info.This());
  return T1::New((obj->*func)());

}
