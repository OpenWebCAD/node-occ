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
#include <nan.h>

#ifdef Handle_was_defined
#define   Handle(ClassName)      Handle_##ClassName
#endif
#define   occHandle(ClassName)      Handle_##ClassName
//xx using namespace v8;


template<class T> bool IsInstanceOf(v8::MaybeLocal<v8::Value> obj) {
  if (obj.IsEmpty()) {
    return false;
  }
  v8::Local<v8::FunctionTemplate> _template = Nan::New(T::_template);
  return _template->HasInstance(obj.ToLocalChecked());
}

template<class T> bool IsInstanceOf(v8::Local<v8::Value> obj) {
	v8::Local<v8::FunctionTemplate> _template = Nan::New(T::_template);
	return _template->HasInstance(obj);

}
template<class T> bool IsInstanceOf(v8::Handle<v8::Object> obj) {
	v8::Local<v8::FunctionTemplate> _template = Nan::New(T::_template);
	return _template->HasInstance(obj);
}



//template<class N,class T> v8::Local<N> c(T e) { return v8::Local<N>(e); }
template<class T> v8::Local<T> c(v8::Local<T>  e) { return e; }
inline v8::Local<v8::String> c(Nan::MaybeLocal<v8::String> e) { return e.ToLocalChecked(); }



template<class T,typename T1,typename T2,T2 (T::*func)() >
NAN_GETTER(ee)
{
    //xx NanScope();
    if (info.This().IsEmpty() ) {
        info.GetReturnValue().SetUndefined();
		return;
    }

    if (info.This()->InternalFieldCount() == 0 ) {
		info.GetReturnValue().SetUndefined();
		return;
	}

    T* obj = node::ObjectWrap::Unwrap<T>(info.This());

    try {

		auto val = (obj->*func)();
		info.GetReturnValue().Set(c(Nan::New<T1>(val)) );

    } catch(...) {
      return Nan::ThrowError("exception caught in C++ code");
	}
}

// proto is an ObjectTemplate
#define EXPOSE_METHOD(ClassName,staticMethod) \
  Nan::SetTemplate(proto,#staticMethod, Nan::New<v8::FunctionTemplate>(staticMethod))

// proto->Set(#staticMethod,v8::FunctionTemplate::New(staticMethod));
// NODE_SET_PROTOTYPE_METHOD(proto,#staticMethod,staticMethod);
//FunctionTemplate::New(staticMethod)->GetFunction());
// proto->Set(Nan::New(#staticMethod),FunctionTemplate::New(staticMethod)->GetFunction());

#define __EXPOSE_READ_ONLY_PROPERTY(element,staticMethod,name) \
    Nan::SetAccessor(element,Nan::New(#name).ToLocalChecked(), &staticMethod,  0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly)

#define EXPOSE_READ_ONLY_PROPERTY(staticMethod,name) \
    __EXPOSE_READ_ONLY_PROPERTY(proto,staticMethod,name)

#define EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(proto, (ee< ClassName, v8::Boolean, bool, &ClassName::name>) , name )

#define EXPOSE_READ_ONLY_PROPERTY_INTEGER(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(proto, (ee<ClassName,v8::Integer,int,&ClassName::name>),name)

#define EXPOSE_READ_ONLY_PROPERTY_DOUBLE(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(proto, (ee<ClassName,v8::Number,double,&ClassName::name>),name)

#define EXPOSE_READ_ONLY_PROPERTY_CONST_STRING(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(proto, (ee<ClassName,v8::String,const char*,&ClassName::name>),name)


#define REXPOSE_READ_ONLY_PROPERTY_DOUBLE(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(info.This(),(ee< ClassName, v8::Number, double, &ClassName::name>) , name )
#define REXPOSE_READ_ONLY_PROPERTY_BOOLEAN(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(info.This(),(ee< ClassName, v8::Boolean, bool, &ClassName::name>) , name )
#define REXPOSE_READ_ONLY_PROPERTY_INTEGER(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(info.This(), (ee<ClassName,v8::Integer,int,&ClassName::name>),name)
#define REXPOSE_READ_ONLY_PROPERTY_CONST_STRING(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(info.This(), (ee<ClassName,v8::String,const char*,&ClassName::name>),name)
/**
 *  Extracts a C string from a V8 Utf8Value.
 * <code>
 *    v8::String::Utf8Value str(args[i]);
 *    const char* cstr = ToCString(str);
 * </code>
 */
inline const char* ToCString(const v8::String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}

// TO DO => SCRAP
#define _NAN_METHOD_ARGS const Nan::FunctionCallbackInfo<v8::Value>& info
#define NanObjectWrapHandle(t) (t->handle())

