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
using namespace v8;

template<class T,typename T1,typename T2,T2 (T::*func)() >
NAN_GETTER(ee)
{
    NanScope();

    if (args.This().IsEmpty() ) {
        NanReturnUndefined();
    }

    if (args.This()->InternalFieldCount() == 0 ) {
        NanReturnUndefined();
    }

    T* obj = node::ObjectWrap::Unwrap<T>(args.This());

    try {
    
      NanReturnValue(NanNew<T1>((obj->*func)()));

    } catch(...) {
      NanThrowError("exception caught in C++ code");
      NanReturnUndefined();
    }
}

// proto is an ObjectTemplate
#define EXPOSE_METHOD(ClassName,staticMethod) \
  NanSetTemplate(proto,#staticMethod, NanNew<v8::FunctionTemplate>(staticMethod))

// proto->Set(#staticMethod,v8::FunctionTemplate::New(staticMethod));
// NODE_SET_PROTOTYPE_METHOD(proto,#staticMethod,staticMethod);
//FunctionTemplate::New(staticMethod)->GetFunction());
// proto->Set(NanNew(#staticMethod),FunctionTemplate::New(staticMethod)->GetFunction());

#define __EXPOSE_READ_ONLY_PROPERTY(element,staticMethod,name) \
    element->SetAccessor(NanNew(#name), &staticMethod,  0,Handle<v8::Value>(),DEFAULT,ReadOnly)


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
    __EXPOSE_READ_ONLY_PROPERTY(args.This(),(ee< ClassName, v8::Number, double, &ClassName::name>) , name )
#define REXPOSE_READ_ONLY_PROPERTY_BOOLEAN(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(args.This(),(ee< ClassName, v8::Boolean, bool, &ClassName::name>) , name )
#define REXPOSE_READ_ONLY_PROPERTY_INTEGER(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(args.This(), (ee<ClassName,v8::Integer,int,&ClassName::name>),name)

#define REXPOSE_READ_ONLY_PROPERTY_CONST_STRING(ClassName,name) \
    __EXPOSE_READ_ONLY_PROPERTY(args.This(), (ee<ClassName,v8::String,const char*,&ClassName::name>),name)
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

