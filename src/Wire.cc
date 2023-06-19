#include "Wire.h"

int Wire::numVertices()
{
  if (this->wire().IsNull())
    return 0;
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(this->wire(), TopAbs_VERTEX, anIndices);
  return anIndices.Extent();
}

int Wire::numEdges()
{
  if (this->wire().IsNull())
    return 0;
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(this->wire(), TopAbs_EDGE, anIndices);
  return anIndices.Extent();
}

bool Wire::isClosed()
{
  if (this->wire().IsNull())
    return false;
  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(this->wire(), aV1, aV2);
  if (!aV1.IsNull() && !aV2.IsNull() && aV1.IsSame(aV2))
    return true;
  return false;
}

const TopoDS_Shape &Wire::shape() const { return m_wire; }
void Wire::setShape(const TopoDS_Shape &shape) { m_wire = TopoDS::Wire(shape); }

Nan::Persistent<v8::FunctionTemplate> Wire::_template;

const char *toString(BRepBuilderAPI_WireError err)
{
  switch (err)
  {
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
  return "<unknown error>";
}

NAN_METHOD(Wire::NewInstance) { _NewInstance<Wire>(info); }

template <class T>
void addElement(T info, BRepBuilderAPI_MakeWire &mkWire)
{
  Edge *edge = DynamicCast<Edge>(info);
  Wire *wire = DynamicCast<Wire>(info);

  if (edge)
  {
    // IsInstanceOf<Edge>(info[i]->ToObject())) {
    // xx Edge* edge = Nan::ObjectWrap::Unwrap<Edge>(info[i]->ToObject());
    mkWire.Add(edge->edge());
    // Xx statusIsDone = mkWire.IsDone();
    // err = mkWire.Error();
  }
  else if (wire)
  {
    mkWire.Add(wire->wire());
    // Xx statusIsDone = mkWire.IsDone();
    //  err = mkWire.Error();
  }
  else
  {
    auto mesg = std::string("invalid arguement: expecting a Wire or an Edge");
    Nan::ThrowError(mesg.c_str());
  }
}
NAN_METHOD(Wire::New)
{
  if (!info.IsConstructCall())
  {
    return Nan::ThrowError(" use new occ.Wire() to construct a Wire");
  }

  Wire *pThis = new Wire();
  pThis->Wrap(info.This());
  pThis->InitNew(info);

  if (info.Length() == 0)
  {
    // this is a empty wire
    info.GetReturnValue().Set(info.This());
    return;
  }

  BRepBuilderAPI_MakeWire mkWire;
  if (info.Length() == 1 && info[0]->IsArray())
  {
    // we expect an array of Wire or Edge
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(info[0]);

    for (int i = 0; i < arr->Length(); i++)
    {
      auto e = Nan::Get(arr, i).ToLocalChecked();
      addElement(e, mkWire);
    }
  }
  else
  {
    for (int i = 0; i < info.Length(); i++)
    {
      addElement(info[i], mkWire);
    }
  }

  // Xx Standard_Boolean statusIsDone = false;
  BRepBuilderAPI_WireError err = BRepBuilderAPI_WireDone;
  err = mkWire.Error();
  if (BRepBuilderAPI_WireDone == err)
  {
    pThis->setShape(mkWire.Wire());
  }
  else
  {
    std::string mesg =
        std::string("Invalid Wire err:=") + toString(mkWire.Error());
    return Nan::ThrowError(mesg.c_str());
  }
  info.GetReturnValue().Set(info.This());
}

v8::Local<v8::Object> Wire::Clone() const
{
  Wire *obj = new Wire();
  v8::Local<v8::Object> instance = makeInstance(_template);
  obj->Wrap(instance);
  obj->setShape(this->shape());
  return instance;
}

NAN_METHOD(Wire::InitNew)
{
  Base::InitNew(info);
  REXPOSE_READ_ONLY_PROPERTY_INTEGER(Wire, numVertices);
  REXPOSE_READ_ONLY_PROPERTY_INTEGER(Wire, numEdges);
  REXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Wire, isClosed);
}

void Wire::Init(v8::Local<v8::Object> target)
{
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl =
      Nan::New<v8::FunctionTemplate>(Wire::New);
  tpl->SetClassName(Nan::New("Wire").ToLocalChecked());

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  _template.Reset(tpl);

  // Prototype
  v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();

  Base::InitProto(proto);

  EXPOSE_METHOD(Wire, getEdges);
  EXPOSE_METHOD(Wire, getVertices);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Wire, numVertices);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Wire, numEdges);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Wire, isClosed);

  Nan::Set(target, Nan::New("Wire").ToLocalChecked(),
           Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Wire::getEdges)
{
  Wire *pThis = UNWRAP(Wire);
  auto arr = extract_shapes_as_javascript_array(pThis, TopAbs_EDGE);
  info.GetReturnValue().Set(arr);
}
NAN_METHOD(Wire::getVertices)
{
  Wire *pThis = UNWRAP(Wire);
  auto arr = extract_shapes_as_javascript_array(pThis, TopAbs_VERTEX);
  info.GetReturnValue().Set(arr);
}
