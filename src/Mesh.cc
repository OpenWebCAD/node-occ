#include "Mesh.h"

#include <set>
#include <list>
#include <algorithm>
#include <map>
#include <assert.h>



Mesh::Mesh()
{
}
   
Handle<Value> Mesh::New(const Arguments& args)
{
  HandleScope scope;


  Mesh* obj = new Mesh();

  args.This()->Set(String::NewSymbol("vertices"),   Object::New());
  args.This()->Set(String::NewSymbol("normals"),    Object::New());
  args.This()->Set(String::NewSymbol("edgeindices"),Object::New());
  args.This()->Set(String::NewSymbol("triangles"),  Object::New());
  obj->Wrap(args.This());
  return args.This();
}

Persistent<FunctionTemplate> Mesh::constructor;


/*static*/void Mesh::Init(Handle<Object> target)
{

 //xx constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Mesh::New));

  // Prepare constructor template
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Mesh::New));
  constructor->SetClassName(String::NewSymbol("Mesh"));

  // object has one internal filed ( the C++ object)
  constructor->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Mesh,numTriangles);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Mesh,numEdges);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Mesh,numVertices);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Mesh,numNormals);

  // other Mesh prototype members are defined in the mesh.js script

  target->Set(String::NewSymbol("Mesh"), constructor->GetFunction());

}

int Mesh::extractFaceMesh(const TopoDS_Face& face, bool qualityNormals)
{

    size_t vsize = this->vertices.size();
    std::vector<gp_Vec> normals;
    bool reversed = false;
    Coord3f vert;
    Coord3f norm;
    Triangle3i tri;
    
    try {
        
        if(face.IsNull()) {
           StdFail_NotDone::Raise("Face is Null");
        }
    
        TopLoc_Location loc;
        occHandle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, loc);
        
        if(triangulation.IsNull()) {
            StdFail_NotDone::Raise("No triangulation created");
        }
        
        gp_Trsf tr = loc;
        const TColgp_Array1OfPnt& narr = triangulation->Nodes();

        Standard_Real x,y,z;

        for (int i = 1; i <= triangulation->NbNodes(); i++) {

            const gp_Pnt& pnt = narr(i);
            x = pnt.X();
            y = pnt.Y();
            z = pnt.Z();
            tr.Transforms(x,y,z);
            
            vert.x = (float)x;
            vert.y = (float)y;
            vert.z = (float)z;

            this->vertices.push_back(vert);
            
            // ensure we have normals for all vertices
            norm.x = 0.f;
            norm.y = 0.f;
            norm.z = 0.f;
            this->normals.push_back(norm);
            
            if (!qualityNormals) {
                normals.push_back(gp_Vec(0.0,0.0,0.0)); 
            }
        }
        
        if (face.Orientation() == TopAbs_REVERSED)
            reversed = true;
        
        const Poly_Array1OfTriangle& triarr = triangulation->Triangles();
        for (int i = 1; i <= triangulation->NbTriangles(); i++)
        {
            Poly_Triangle pt = triarr(i);
            Standard_Integer n1,n2,n3;
            
            if(reversed)
                pt.Get(n2,n1,n3);
            else
                pt.Get(n1,n2,n3);
            
            // make sure that we don't process invalid triangle
            if (n1 == n2 || n2 == n3 || n3 == n1)
                continue;
                
            // Calculate face normal
            const gp_Pnt& P1 = narr(n1);
            const gp_Pnt& P2 = narr(n2);
            const gp_Pnt& P3 = narr(n3);
            
            gp_Vec V1(P3.X() - P1.X(), P3.Y() - P1.Y(), P3.Z() - P1.Z());
            if (V1.SquareMagnitude() < 1.0e-10)
                // Degenerated triangle
                continue;
            
            gp_Vec V2(P2.X() - P1.X(), P2.Y() - P1.Y(), P2.Z() - P1.Z());
            if (V2.SquareMagnitude() < 1.0e-10)
                // Degenerated triangle
                continue;
            
            gp_Vec normal = V1.Crossed(V2);
            if (normal.SquareMagnitude() < 1.0e-10)
                // Degenerated triangle
                continue;
            
            tri.i = (int)vsize + n1 - 1;
            tri.j = (int)vsize + n2 - 1;
            tri.k = (int)vsize + n3 - 1;
            this->triangles.push_back(tri);
            
            if (!qualityNormals) {
                normals[n1 - 1] = normals[n1 - 1] - normal;
                normals[n2 - 1] = normals[n2 - 1] - normal;
                normals[n3 - 1] = normals[n3 - 1] - normal;
            }
        }
        
        if (qualityNormals) {
            Handle_Geom_Surface surface = BRep_Tool::Surface(face);
            gp_Vec normal;
            for (int i = 0; i < triangulation->NbNodes(); i++)
            {
                vert = this->vertices[vsize + i];
                gp_Pnt vertex(vert.x, vert.y, vert.z);
                GeomAPI_ProjectPointOnSurf SrfProp(vertex, surface);
                Standard_Real fU, fV;
                SrfProp.Parameters(1, fU, fV);

                GeomLProp_SLProps faceprop(surface, fU, fV, 2, gp::Resolution());
                normal = faceprop.Normal();
                
                if (normal.SquareMagnitude() > 1.0e-10)
                    normal.Normalize();
                
                if (reversed) {
                    norm.x = (float)-normal.X();
                    norm.y = (float)-normal.Y();
                    norm.z = (float)-normal.Z();
                } else {
                    norm.x = (float)normal.X();
                    norm.y = (float)normal.Y();
                    norm.z = (float)normal.Z();
                }
                this->normals[vsize + i] = norm;
            }
        } else {
            // Normalize vertex normals
            for (int i = 0; i < triangulation->NbNodes(); i++)
            {
                gp_Vec normal = normals[i];
                if (normal.SquareMagnitude() > 1.0e-10)
                    normal.Normalize();
                
                norm.x = (float)normal.X();
                norm.y = (float)normal.Y();
                norm.z = (float)normal.Z();
                this->normals[vsize + i] = norm;
            }
        }
        
        // extract edge indices from mesh
        std::set<int> seen;
        for (unsigned int i = 0; i < this->edgehash.size(); i++)
            seen.insert(this->edgehash[i]);
        
        size_t lastSize = this->edgeindices.size();
        TopExp_Explorer ex0, ex1;
        for (ex0.Init(face, TopAbs_WIRE); ex0.More(); ex0.Next()) {
            const TopoDS_Wire& wire = TopoDS::Wire(ex0.Current());
            
            for (ex1.Init(wire, TopAbs_EDGE); ex1.More(); ex1.Next()) {
                const TopoDS_Edge& edge = TopoDS::Edge(ex1.Current());
                
                // skip degenerated edge
                if (BRep_Tool::Degenerated(edge))
                    continue;
                
                // skip edge if it is a seam
                if (BRep_Tool::IsClosed(edge, face))
                    continue;
                    
                int hash = edge.HashCode(std::numeric_limits<int>::max());
                if (seen.count(hash) == 0) {
                    occHandle(Poly_PolygonOnTriangulation) edgepoly = BRep_Tool::PolygonOnTriangulation(edge, triangulation, loc);
                    if (edgepoly.IsNull()) {
                        continue;
                    }
                    seen.insert(hash);
                    this->edgehash.push_back(hash);
                    this->edgeranges.push_back((int)this->edgeindices.size());
                    
                    const TColStd_Array1OfInteger& edgeind = edgepoly->Nodes();
                    for (int i=edgeind.Lower();i <= edgeind.Upper();i++) {
                        const unsigned int idx = (unsigned int)edgeind(i);
                        this->edgeindices.push_back((int)(vsize + idx - 1));
                    }
                    
                    this->edgeranges.push_back((int)(this->edgeindices.size() - lastSize));
                    lastSize = this->edgeindices.size();
                }
            }
        }
        
    } catch(Standard_Failure&) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        const Standard_CString msg = e->GetMessageString();
        if (msg != NULL && strlen(msg) > 1) {
            setErrorMessage(msg);
        } else {
            setErrorMessage("Failed to mesh object");
        }
        return 0;
    }
    optimize();
	updateJavaScriptArray();
    return 1;
}

template <class T> struct _typeMatcher{	 static ExternalArrayType get() ;};

template <> struct _typeMatcher<float>       {	static ExternalArrayType get() { return kExternalFloatArray ; } };
template <> struct _typeMatcher<int>         {	static ExternalArrayType get() { return kExternalShortArray ; } };
template <> struct _typeMatcher<unsigned int>{	static ExternalArrayType get() { return kExternalUnsignedShortArray ; } };

template<class T>
void UpdateExternalArray(Handle<Object>& pThis,const char* name,const T* data,size_t _length)
{
	int length = (int)_length;
	
	Handle<Object> extArray(pThis->Get(String::NewSymbol(name))->ToObject());
	if (extArray.IsEmpty()) {
			ThrowException(Exception::Error(String::New(" error in UpdateExternalArray")));

	}
	extArray->SetIndexedPropertiesToExternalArrayData(const_cast<T*>(data), _typeMatcher<T>::get(), length);

	extArray->Set(String::NewSymbol("length"), Int32::New(length), DontDelete);
	assert(	 extArray->GetIndexedPropertiesExternalArrayDataLength() ==   length);
	assert(	 extArray->Get(String::NewSymbol("length"))->ToInt32()->Uint32Value() ==   length);
}

void Mesh::updateJavaScriptArray()
{						
    assert(sizeof(triangles[0])==sizeof(int)*3);
	HandleScope scope;		   
	Handle<Object> pThis = this->handle_;
	UpdateExternalArray(pThis, "vertices"    ,&vertices.data()[0].x  ,vertices.size()*3);
	UpdateExternalArray(pThis, "normals"     ,&normals.data()[0].x   ,normals.size()*3);
	UpdateExternalArray(pThis, "triangles"   ,&triangles.data()[0].i ,triangles.size()*3);
	UpdateExternalArray(pThis, "edgeindices" ,&edgeindices.data()[0] ,edgeindices.size());
}


float square(float b) { return b*b;}

float distance2( const Coord3f& a,const Coord3f& b) 
{
	return square(a.x -b.x) + square(a.y -b.y)	+ square(a.y -b.y);
}
int findPt(const std::vector<Coord3f>& arrayPts, const Coord3f pt,double tolerance)
{							   
	
	double tolerance2 =tolerance*tolerance;
	int i=0;
	for (std::vector<Coord3f>::const_iterator it=arrayPts.begin();it!=arrayPts.end();it++) {
		const Coord3f& point = (*it);
		if ( distance2(point,pt) < tolerance2)  {
			return i;
		}
		i++;
	}
	return -1;
}					
void Mesh::optimize()
{
#if 0
	double tolerance =1E-6;

 	std::map<int,int> indexOldNewMap;
	std::vector<Coord3f> newPoints;
	newPoints.reserve(vertices.size());
	int i=0;
	for (std::vector<Coord3f>::const_iterator it=vertices.begin();it!=vertices.end();it++) 
	{
		int pos = findPt(newPoints,*it,tolerance);
		if (pos == -1) {
			newPoints.push_back(*it);
			indexOldNewMap[i] =   newPoints.size()-1;
		}
		i++;
	}
#endif


}
