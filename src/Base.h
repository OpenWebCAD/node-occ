#pragma once
#include "NodeV8.h"
#include "OCC.h"

#include "vector"


class Base : public node::ObjectWrap {
public:
    int hashCode();
    bool isNull();
    bool isValid();
    const char* shapeType();
    const char* orientation();
    bool fixShape();

    virtual const TopoDS_Shape& shape() const = 0;
    virtual void setShape(const TopoDS_Shape& ) = 0;
    virtual Local<Object>  Clone() =0;
    virtual Base* Unwrap(v8::Local<v8::Object> obj) =0;//  { return node::ObjectWrap::Unwrap<Edge>(obj); }
public:
// Methods exposed to JavaScripts
    static Handle<Value> translate(const Arguments& args);
    static Handle<Value> rotate(const Arguments& args);
    static Handle<Value> scale(const Arguments& args);
    static Handle<Value> mirror(const Arguments& args);
    static Handle<Value> applyTransform(const Arguments& args);
    static Handle<Value> transformed(const Arguments& args);
    static Handle<Value> fixShape(const Arguments&);

    static Handle<v8::Value> getBoundingBox(const v8::Arguments& args);

    static void  InitProto(Handle<ObjectTemplate> target);
};

v8::Local<v8::Object> buildEmptyWrapper(TopAbs_ShapeEnum type);
v8::Local<v8::Object> buildWrapper(const TopoDS_Shape shape);

#define CATCH_AND_RETHROW(message)									\
   catch(Standard_Failure& ) {										\
        Handle_Standard_Failure e = Standard_Failure::Caught();		\
        Standard_CString msg = e->GetMessageString();				\
        if (msg == NULL || strlen(msg) < 1) {						\
            msg = message;											\
        }															\
		ThrowException(Exception::Error(String::New(msg)));		\
    }																\
 
template<class ClassType>
size_t extractArgumentList(const Arguments& args,std::vector<ClassType*>& elements)
{
    elements.reserve(elements.size()+args.Length());

    for (int i=0; i<args.Length(); i++) {
        if (ClassType::constructor->HasInstance(args[i]->ToObject())) {
            elements.push_back(node::ObjectWrap::Unwrap<ClassType>(args[i]->ToObject()));
        }
    }
    return elements.size();
}
template<class ClassType>
size_t extractArray(const Handle<Value>& value,std::vector<ClassType*>& elements)
{
    if (value->IsArray())  {
        Handle<Array> arr = Handle<Array>::Cast(value);
        int length = arr->Length();
        elements.reserve(elements.size()+length);
        for (int i=0; i<length; i++) {
            Handle<Object> obj = arr->Get(i)->ToObject();
            if (ClassType::constructor->HasInstance(obj)) {
                elements.push_back(node::ObjectWrap::Unwrap<ClassType>(obj));
            }
        }
    }
    return elements.size();
}





template <typename OBJECT>
OBJECT* DynamicCast(const Handle<Value>& value)
{
    if (value.IsEmpty()) return 0;
    if (!value->IsObject()) return 0;
    if (OBJECT::constructor->HasInstance(value->ToObject())) {
        return node::ObjectWrap::Unwrap<OBJECT>(value->ToObject());
    }
    return 0;
}
template <typename ObjType1,typename ObjType2>
Base*  DynamicCast(const Handle<Value>& value)
{
    ObjType1* obj = DynamicCast<ObjType1>(value);
    if (obj) return obj;
    return 	 DynamicCast<ObjType2>(value);
}
