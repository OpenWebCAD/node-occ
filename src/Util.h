#pragma once
#include "OCC.h"
#include "NodeV8.h"


inline v8::Local<v8::Object>  makeInstance(Nan::Persistent<v8::FunctionTemplate>& _template) {
  return Nan::NewInstance(Nan::GetFunction(Nan::New(_template)).ToLocalChecked()).ToLocalChecked();
}

template <class T> inline double extract_double(const v8::Local<T>& a) {
  return Nan::To<double>(a).FromJust();
}

void ReadDouble(const v8::Local<v8::Value>& _v,double& value);

void ReadInt(v8::Local<v8::Object> obj,const char* name,int* retValue,int defaultValue);
void ReadDouble(v8::Local<v8::Object> obj,const char* name,double* retValue,double defaultValue=0.0);

void ReadPropertyPointFromArray(v8::Local<v8::Array> value,double* x,double* y, double*z );
// void ReadPropertyPoint( Handle<Object> value,const char* name,double* x,double* y, double*z );

void ReadXYZ(v8::Local<v8::Object> value,double* x,double* y,double* z);
void ReadUVW(v8::Local<v8::Object> value,double* x,double* y,double* z);

void ReadPoint(v8::Local<v8::Value> value,double* x,double* y, double*z);
void ReadPoint(v8::Local<v8::Value> value,gp_Pnt* pt);
void ReadDir(v8::Local<v8::Value> value,gp_Dir* pt);
void ReadVector(v8::Local<v8::Value> value,gp_Vec* pt);

void ReadRotationFromArgs(_NAN_METHOD_ARGS,gp_Trsf& trans);


enum ArrayType {
  A_SByte, A_UInt8, A_Int16, A_UInt16, A_Int32, A_UInt32, A_Float32, A_Float64
};

inline const char* ArrayTypeToString(ArrayType type) {
  const char* name = 0;
  switch (type) {
  case A_SByte:    name = "Uint8Array";   break;
  case A_UInt8:    name = "Uint8Array";   break;
  case A_Int16:   name = "Int16Array";   break;
  case A_UInt16:  name = "Uint16Array";  break;
  case A_Int32:   name = "Int32Array";   break;
  case A_UInt32:  name = "Uint32Array";  break;
  case A_Float32: name = "Float32Array"; break;
  case A_Float64: name = "Float64Array"; break;
  default:
    name = 0;
  }
  return name;
}
inline int ArrayTypeSize(ArrayType type) {

  switch (type) {
  case A_SByte:   return 1; break;
  case A_UInt8:   return 1;   break;
  case A_Int16:   return 2; break;
  case A_UInt16:  return 2; break;
  case A_Int32:   return 4; break;
  case A_UInt32:  return 4; break;
  case A_Float32: return 4; break;
  case A_Float64: return 8; break;
  default:
    return 0;
  }
}

inline v8::Local<v8::Value> makeArrayBuffer(int length) {

  v8::Local<v8::Value> val = Nan::New("ArrayBuffer").ToLocalChecked();

  if (val.IsEmpty() || !val->IsFunction()) {
    Nan::ThrowError("Error getting ArrayBuffer constructor");
    return val;
  }

  v8::Local<v8::Function> constructor = val.As<v8::Function>();
  v8::Local<v8::Value> size = Nan::New<v8::Integer>(length);

  Nan::MaybeLocal<v8::Value> array_buffer = Nan::CallAsConstructor(constructor, 1, &size);
  return array_buffer.ToLocalChecked();
}

#define IS_FLOAT32ARRAY(value)               (value->IsFloat32Array() && (value.As<v8::Float32Array>()->Length() == 2))
#define GET_FLOAT32ARRAY_DATA(value)         (float*)(static_cast<char*>(value.As<v8::Float32Array>()->Buffer()->GetContents().Data()) + value.As<v8::Float32Array>()->ByteOffset())
#define IS_FLOAT32ARRAY_ARRAY(value)         (value->IsFloat32Array() && ((value.As<v8::Float32Array>()->Length() % 2) == 0))
#define GET_FLOAT32ARRAY_ARRAY_DATA(value)   GET_FLOAT32ARRAY_DATA(value)
#define GET_FLOAT32ARRAY_ARRAY_LENGTH(value) (value.As<v8::Float32Array>()->Length())


#define IS_INT32ARRAY(value)                (value->IsInt32Array() && (value.As<v8::Int32Array>()->Length() == 2))
#define GET_INT32ARRAY_DATA(value)          (int*)(static_cast<char*>(value.As<v8::Int32Array>()->Buffer()->GetContents().Data()) + value.As<v8::Int32Array>()->ByteOffset())
#define IS_INT32ARRAY_ARRAY(value)          (value->IsInt32Array() && ((value.As<v8::Int32Array>()->Length() % 2) == 0))
#define GET_INT32ARRAY_ARRAY_DATA(value)    GET_INT32ARRAY_DATA(value)
#define GET_INT32ARRAY_ARRAY_LENGTH(value)  (value.As<v8::Int32Array>()->Length())

#define IS_UINT32ARRAY(value)               (value->IsUint32Array() && (value.As<v8::Uint32Array>()->Length() == 2))
#define GET_UINT32ARRAY_DATA(value)         (unsigned int*)(static_cast<char*>(value.As<v8::Uint32Array>()->Buffer()->GetContents().Data()) + value.As<v8::Uint32Array>()->ByteOffset())
#define IS_UINT32ARRAY_ARRAY(value)         (value->IsUint32Array() && ((value.As<v8::Uint32Array>()->Length() % 2) == 0))
#define GET_UINT32ARRAY_ARRAY_DATA(value)   GET_UINT32ARRAY_DATA(value)
#define GET_UINT32ARRAY_ARRAY_LENGTH(value) (value.As<v8::Uint32Array>()->Length())


#define IS_UINT16ARRAY(value)               (value->IsUint16Array() && (value.As<v8::Uint16Array>()->Length() == 2))
#define GET_UINT16ARRAY_DATA(value)         (unsigned short*)(static_cast<char*>(value.As<v8::Uint16Array>()->Buffer()->GetContents().Data()) + value.As<v8::Uint16Array>()->ByteOffset())
#define IS_UINT16ARRAY_ARRAY(value)         (value->IsUint32Array() && ((value.As<v8::Uint16Array>()->Length() % 2) == 0))
#define GET_UINT16ARRAY_ARRAY_DATA(value)   GET_UINT16ARRAY_DATA(value)
#define GET_UINT16ARRAY_ARRAY_LENGTH(value) (value.As<v8::Uint16Array>()->Length())

#define IS_UINT8ARRAY(value)               (value->IsUint8Array() && (value.As<v8::Uint8Array>()->Length() == 2))
#define GET_UINT8ARRAY_DATA(value)         (unsigned char*)(static_cast<char*>(value.As<v8::Uint8Array>()->Buffer()->GetContents().Data()) + value.As<v8::Uint8Array>()->ByteOffset())
#define IS_UINT8ARRAY_ARRAY(value)         (value->IsUint8Array() && ((value.As<v8::Uint8Array>()->Length() % 2) == 0))
#define GET_UINT8ARRAY_ARRAY_DATA(value)   GET_UINT8ARRAY_DATA(value)
#define GET_UINT8ARRAY_ARRAY_LENGTH(value) (value.As<v8::Uint8Array>()->Length())


inline v8::Local<v8::Object> makeTypedArray(ArrayType type, uint32_t length) {

  const char *name = ArrayTypeToString(type);
  if (!name) {
    Nan::ThrowError("Unsupported array type");
    return Nan::New<v8::Object>();
  }

  auto typedArrayConstructor = Nan::Get(Nan::GetCurrentContext()->Global(),Nan::New(name).ToLocalChecked()).ToLocalChecked();
  
  if (typedArrayConstructor.IsEmpty() || !typedArrayConstructor->IsFunction()) {
    Nan::ThrowError("Error getting typed array constructor");
    return Nan::New<v8::Object>();
  }

  v8::Local<v8::Function> constructor = Nan::To<v8::Function>(typedArrayConstructor).ToLocalChecked();
  
  const int argc = 1;
  v8::Local<v8::Value> argv[1] = { Nan::New(length) };
  v8::Local<v8::Object> array = constructor->NewInstance(Nan::GetCurrentContext(),argc, argv).ToLocalChecked();

  if (array.IsEmpty() || !array->IsObject()) {
    Nan::ThrowError("Error creating TypedArray");
    return v8::Local<v8::Object>();
  }
  return array;
}


inline v8::Local<v8::Object> makeFloat32Array(const float* data, uint32_t length) {
  v8::Local<v8::Object> array = makeTypedArray(A_Float32, length);
  float* dest = GET_FLOAT32ARRAY_ARRAY_DATA(array);
  memcpy(dest, data, length * sizeof(data[0]));
  return array;
}
inline v8::Local<v8::Object> makeInt32Array(const int* data, uint32_t length) {
  v8::Local<v8::Object> array = makeTypedArray(A_Int32, length);
  int* dest = GET_INT32ARRAY_ARRAY_DATA(array);
  memcpy(dest, data, length * sizeof(data[0]));
  return array;
}
inline v8::Local<v8::Object> makeUint32Array(const unsigned int* data, uint32_t length) {
  v8::Local<v8::Object> array = makeTypedArray(A_UInt32, length);
  unsigned int* dest = GET_UINT32ARRAY_ARRAY_DATA(array);
  memcpy(dest, data, length * sizeof(data[0]));
  return array;
}
inline v8::Local<v8::Object> makeUint16Array(const unsigned short* data, uint32_t length) {
  v8::Local<v8::Object> array = makeTypedArray(A_UInt16, length);
  unsigned short* dest = GET_UINT16ARRAY_ARRAY_DATA(array);
  memcpy(dest, data, length * sizeof(data[0]));
  return array;
}
inline v8::Local<v8::Object> makeUint8Array(const unsigned char* data, uint32_t length) {
  v8::Local<v8::Object> array = makeTypedArray(A_UInt8, length);
  unsigned char* dest = GET_UINT8ARRAY_ARRAY_DATA(array);
  memcpy(dest, data, length * sizeof(data[0]));
  return array;
}



inline v8::Local<v8::Object> _makeTypedArray(const float* data, int length) {
  return makeFloat32Array(data, length);
}

inline v8::Local<v8::Object> _makeTypedArray(const int* data, int length) {
  return makeInt32Array(data, length);
}
inline v8::Local<v8::Object> _makeTypedArray(const unsigned int* data, int length) {
  return makeUint32Array(data, length);
}


template <typename OBJECT>
OBJECT* DynamicCast(const v8::Local<v8::Value>& value)
{
  if (value.IsEmpty()) return 0;
  if (!value->IsObject()) return 0;

  auto o = Nan::To<v8::Object>(value).ToLocalChecked();
  if (IsInstanceOf<OBJECT>(o)) {
    return Nan::ObjectWrap::Unwrap<OBJECT>(o);
  }
  return 0;
}
template <typename ObjType1, typename ObjType2>
ObjType2* DynamicCast(const v8::Local<v8::Value>& value)
{
  ObjType1* obj = DynamicCast<ObjType1>(value);
  if (obj) return obj;
  return DynamicCast<ObjType2>(value);
}

template<class T> v8::Local<v8::Function> Constructor() {

  auto f = Nan::GetFunction(Nan::New(T::_template)).ToLocalChecked();
  return f;
}

template<class T> NAN_METHOD(_NewInstance) {
    int argc =info.Length();
    auto  argv = new v8::Local<v8::Value>[argc];// = new v8::Local<v8::Value>[argc];
    for (int i=0;i<argc;i++) {
        argv[i] = info[i];
    }
    auto instance = Nan::NewInstance(Constructor<T>(),argc,argv);
    delete [] argv;
    info.GetReturnValue().Set(instance.ToLocalChecked());
}
