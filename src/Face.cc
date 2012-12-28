#include "Face.h"
#include "Wire.h"
#include "Edge.h"
#include "Util.h"


const TopoDS_Shape&  Face::shape() const
{
    return face();
}

void Face::setShape( const TopoDS_Shape& shape)
{
    m_face = TopoDS::Face(shape);
}

int Face::numWires()
{
    if(shape().IsNull()) return 0;
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(shape(), TopAbs_WIRE, anIndices);
    return anIndices.Extent();
}

bool Face::fixShape()
{
    return true;
}

double Face::area()
{
    GProp_GProps prop;
    BRepGProp::SurfaceProperties(shape(), prop);
    return prop.Mass();
}


// DVec Face::inertia() {
//    DVec ret;
//    GProp_GProps prop;
//    BRepGProp::SurfaceProperties(this->getShape(), prop);
//    gp_Mat mat = prop.MatrixOfInertia();
//    ret.push_back(mat(1,1)); // Ixx
//    ret.push_back(mat(2,2)); // Iyy
//    ret.push_back(mat(3,3)); // Izz
//    ret.push_back(mat(1,2)); // Ixy
//    ret.push_back(mat(1,3)); // Ixz
//    ret.push_back(mat(2,3)); // Iyz
//    return ret;
//}
//
//OCCStruct3d Face::centreOfMass() {
//    OCCStruct3d ret;
//    GProp_GProps prop;
//    BRepGProp::SurfaceProperties(this->getShape(), prop);
//    gp_Pnt cg = prop.CentreOfMass();
//    ret.x = cg.X();
//    ret.y = cg.Y();
//    ret.z = cg.Z();
//    return ret;
//}
//


Persistent<FunctionTemplate> Face::constructor;




bool Face::buildFace(std::vector<Wire*>& wires)
{
    if (wires.size()==0) return false;

    // checling that all wires are closed
    for (size_t i = 0; i < wires.size(); i++) {
        if (!wires[i]->isClosed()) {
            ThrowException(Exception::TypeError(String::New("Some of the wires are not closed")));
            return false;
        }
    }

    try {
        const TopoDS_Wire& outerwire = wires[0]->wire();

        BRepBuilderAPI_MakeFace MF(outerwire);

        // add optional holes
        for (unsigned i = 1; i < wires.size(); i++) {
            const TopoDS_Wire& wire = wires[i]->wire();

            if (wire.Orientation() != outerwire.Orientation()) {
                MF.Add(TopoDS::Wire(wire.Reversed()));
            } else {
                MF.Add(wire);
            }
        }
        this->setShape(MF.Shape());

        // possible fix shape
        if (!this->fixShape()) {
            StdFail_NotDone::Raise("Shapes not valid");
        }

    }
    CATCH_AND_RETHROW("Failed to create arc");
    return true;
}

Handle<Value> Face::New(const Arguments& args)
{
    HandleScope scope;

    Face* obj = new Face();
    obj->Wrap(args.This());

    std::vector<Wire*> wires;
    extractArgumentList(args,wires);
    obj->buildFace(wires);

    // return scope.Close(args.This());
    return args.This();
}

Local<Object>  Face::Clone()
{
    HandleScope scope;
    Face* obj = new Face();
    Local<Object> instance = constructor->GetFunction()->NewInstance();
    obj->Wrap(instance);
    obj->setShape(this->shape());
    return scope.Close(instance);
}

void Face::Init(Handle<Object> target)
{
    // Prepare constructor template
    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Face::New));
    constructor->SetClassName(String::NewSymbol("Face"));

    // object has one internal filed ( the C++ object)
    constructor->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

    Base::InitProto(proto);

    EXPOSE_READ_ONLY_PROPERTY_INTEGER(Face,numWires);
    EXPOSE_READ_ONLY_PROPERTY_DOUBLE(Face,area);

    target->Set(String::NewSymbol("Face"), constructor->GetFunction());
}
