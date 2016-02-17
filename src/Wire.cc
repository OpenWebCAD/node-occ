#include "Wire.h"


int Wire::numVertices()
{
  if (this->wire().IsNull()) return 0;
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(this->wire(), TopAbs_VERTEX, anIndices);
  return anIndices.Extent();
}

int Wire::numEdges()
{
  if (this->wire().IsNull()) return 0;
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(this->wire(), TopAbs_EDGE, anIndices);
  return anIndices.Extent();
}

bool Wire::isClosed()
{
  if (this->wire().IsNull()) return false;
  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(this->wire(), aV1, aV2);
  if (!aV1.IsNull() && !aV2.IsNull() && aV1.IsSame(aV2))
    return true;
  return false;
}


const TopoDS_Shape& Wire::shape() const
{
  return m_wire;
}
void Wire::setShape(const TopoDS_Shape& shape)
{

  m_wire = TopoDS::Wire(shape);
}

Nan::Persistent<v8::FunctionTemplate> Wire::_template;


const char* toString(BRepBuilderAPI_WireError err)
{
  switch (err) {
  case BRepBuilderAPI_WireDone:
    return "the wire is done";
    break;
  case BRepBuilderAPI_EmptyWire:
    return "the wire is empty";
    break;
  case BRepBuilderAPI_DisconnectedWire:
    return "the wire is disconnected";
    break;
  case BRepBuilderAPI_NonManifoldWire:
    return "the wire is non-manifold";
    break;
  }
  return "";
}



NAN_METHOD(Wire::New)
{
  if (!info.IsConstructCall()) {
    return Nan::ThrowError(" use new occ.Wire() to construct a wire");
  }

  Wire* pThis = new Wire();
  pThis->Wrap(info.This());

  if (info.Length() == 0) {
    // this is a empty wire
    info.GetReturnValue().Set(info.This());
    return;
  }

  BRepBuilderAPI_MakeWire mkWire;

  Standard_Boolean status = false;

  BRepBuilderAPI_WireError err = BRepBuilderAPI_WireDone;

  for (int i = 0; i < info.Length(); i++) {

    if (IsInstanceOf<Edge>(info[i]->ToObject())) {

      Edge* edge = node::ObjectWrap::Unwrap<Edge>(info[i]->ToObject());
      mkWire.Add(edge->edge());

      status = mkWire.IsDone();
      err = mkWire.Error();
    }
    else if (IsInstanceOf<Wire>(info[i]->ToObject())) {

      Wire* wire = node::ObjectWrap::Unwrap<Wire>(info[i]->ToObject());
      mkWire.Add(wire->wire());
      status = mkWire.IsDone();
      err = mkWire.Error();
    }
  }

  err = mkWire.Error();
  if (BRepBuilderAPI_WireDone == err) {
    pThis->setShape(mkWire.Wire());
  }
  else {
    std::string mesg = std::string("Invalid Wire err:=") + toString(mkWire.Error());
    return Nan::ThrowError(mesg.c_str());
  }

  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Object>  Wire::Clone() const
{
  Wire* obj = new Wire();
  v8::Local<v8::Object> instance = Nan::New(_template)->GetFunction()->NewInstance();
  obj->Wrap(instance);
  obj->setShape(this->shape());
  return instance;
}

void Wire::Init(v8::Handle<v8::Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(Wire::New);
  tpl->SetClassName(Nan::New("Wire").ToLocalChecked());

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  Base::InitProto(proto);

  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Wire, numVertices);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Wire, numEdges);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Wire, isClosed);

  target->Set(Nan::New("Wire").ToLocalChecked(), tpl->GetFunction());
}

