// NodeV8
#pragma once
#ifdef _MSC_VER
#pragma warning(disable:4530) // C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#pragma warning(disable:4506)
#pragma warning(disable:4355) // warning C4355: 'this' : used in base member initializer list

#endif

#ifdef Handle
#define Handle_was_defined
#undef Handle
#endif

#include <v8.h>
#include <node.h>

#ifdef Handle_was_defined
#define   Handle(ClassName)      Handle_##ClassName
#endif
#define   occHandle(ClassName)      Handle_##ClassName
using namespace v8;

template<class T,typename T1,typename T2,typename T2 (T::*func)() >  
Handle<Value> ee(Local<String> property,const AccessorInfo &info)
{
  HandleScope scope;
  if (info.This().IsEmpty() ) {
	  return scope.Close(Undefined());
  }
  if (info.This()->InternalFieldCount() == 0 ) {
	  return scope.Close(Undefined());
  }
  T* obj = node::ObjectWrap::Unwrap<T>(info.This());
  return scope.Close(T1::New((obj->*func)()));
}
		   

#define EXPOSE_METHOD(ClassName,staticMethod) \
	proto->Set(String::NewSymbol(#staticMethod),FunctionTemplate::New(staticMethod)->GetFunction());

#define EXPOSE_READ_ONLY_PROPERTY(ClassName,staticMethod,name) \
	proto->SetAccessor(String::NewSymbol(#name), &staticMethod,  0,Handle<v8::Value>(),DEFAULT,ReadOnly)

#define EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(ClassName,name) \
	EXPOSE_READ_ONLY_PROPERTY(proto, (ee< ClassName, Boolean, bool, &ClassName::name>) , name )
					  
#define EXPOSE_READ_ONLY_PROPERTY_INTEGER(ClassName,name) \
	EXPOSE_READ_ONLY_PROPERTY(proto, (ee<ClassName,Integer,int,&ClassName::name>),name)

#define EXPOSE_READ_ONLY_PROPERTY_DOUBLE(ClassName,name) \
	EXPOSE_READ_ONLY_PROPERTY(proto, (ee<ClassName,Number,double,&ClassName::name>),name)

#define EXPOSE_READ_ONLY_PROPERTY_CONST_STRING(ClassName,name) \
	EXPOSE_READ_ONLY_PROPERTY(proto, (ee<ClassName,String,const char*,&ClassName::name>),name)



/** 
 *  Extracts a C string from a V8 Utf8Value.
 * <code>
 *    v8::String::Utf8Value str(args[i]);
 *    const char* cstr = ToCString(str);
 * </code>
 */
inline const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

