#include "Face.h"
#include "Wire.h"
#include "Edge.h"
#include "Util.h"
#include "Mesh.h"


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

bool Face::hasMesh()
{
	TopLoc_Location loc;
	occHandle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(this->face(), loc);
	return triangulation.IsNull()?false:true;
}

std::vector<double> Face::inertia()
{
    std::vector<double> ret;
    GProp_GProps prop;
    BRepGProp::SurfaceProperties(this->shape(), prop);
    gp_Mat mat = prop.MatrixOfInertia();
    ret.push_back(mat(1,1)); // Ixx
    ret.push_back(mat(2,2)); // Iyy
    ret.push_back(mat(3,3)); // Izz
    ret.push_back(mat(1,2)); // Ixy
    ret.push_back(mat(1,3)); // Ixz
    ret.push_back(mat(2,3)); // Iyz
    return ret;
}


const gp_XYZ Face::centreOfMass() const
{

    GProp_GProps prop;
    BRepGProp::SurfaceProperties(this->shape(), prop);
    gp_Pnt cg = prop.CentreOfMass();

    return cg.Coord();
}

bool Face::isPlanar()
{

    Handle_Geom_Surface surf = BRep_Tool::Surface(this->m_face);
    GeomLib_IsPlanarSurface tool(surf);
    return tool.IsPlanar() ? true : false;
}

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
    CATCH_AND_RETHROW("Failed to create a face");
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

Local<Object> Face::Clone()
{
    HandleScope scope;
    Face* obj = new Face();
    Local<Object> instance = constructor->GetFunction()->NewInstance();
    obj->Wrap(instance);
    obj->setShape(this->shape());
    return scope.Close(instance);
}

Handle<Object> Face::NewInstance(const TopoDS_Face& face)
{
    HandleScope scope;
    Face* obj = new Face();
    Local<Object> instance = constructor->GetFunction()->NewInstance();
    obj->Wrap(instance);
    obj->setShape(face);
    return scope.Close(instance);
}

Handle<v8::Value> Face::_mesh(Local<String> property,const AccessorInfo &info)
{
    HandleScope scope;
    if (info.This().IsEmpty()) {
        return scope.Close(Undefined());
    }
    if (info.This()->InternalFieldCount() == 0 ) {
        return scope.Close(Undefined());
    }
    Face* pThis = ObjectWrap::Unwrap<Face>(info.This());
    if (pThis->m_cacheMesh.IsEmpty()) {
        pThis->m_cacheMesh = Persistent<Object>::New(pThis->createMesh(0.001,0.1,true));
    }
    return scope.Close(pThis->m_cacheMesh);
}

Handle<Object> Face::createMesh(double factor, double angle, bool qualityNormals)
{
    HandleScope scope;

    const unsigned argc = 0;
    Handle<v8::Value> argv[1] = {  };
    Local<Object> theMesh = Mesh::constructor->GetFunction()->NewInstance(argc, argv);

    Mesh *mesh =  Mesh::Unwrap<Mesh>(theMesh);

    const TopoDS_Shape& shape = this->shape();

	try {
		// this code assume that the triangulation has been created
		// on the parent object
		mesh->extractFaceMesh(this->face(), qualityNormals);

    } CATCH_AND_RETHROW("Failed to mesh solid ");
    mesh->optimize();
    return scope.Close(theMesh);
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
    EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Face,isPlanar);
    EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Face,hasMesh);
	EXPOSE_READ_ONLY_PROPERTY(Face,_mesh,mesh);
    EXPOSE_TEAROFF(Face,centreOfMass);
    target->Set(String::NewSymbol("Face"), constructor->GetFunction());
}

