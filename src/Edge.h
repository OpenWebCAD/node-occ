#pragma once
#include "Shape.h"

#include "Vertex.h"

class Edge : public Base 
{

public:
	TopoDS_Edge m_edge;
	Edge() { ; }

	bool isSeam(Base *face);

	bool isDegenerated();
	bool isClosed();
	int numVertices();
	double length();

	Edge *copy(bool deepCopy);


	// OCCTesselation *tesselate(double factor, double angle);

	int createLine(Vertex *start, Vertex *end);
	int createArc(Vertex *start, Vertex *end, const gp_Pnt& center);
	int createArc3P(Vertex *start, Vertex *end, const gp_Pnt& middlePoint);

	int createCircle(const gp_Pnt& center , const gp_Dir& normal, double radius);

	//int createEllipse(OCCStruct3d pnt, OCCStruct3d nor, double rMajor, double rMinor);
	//int createHelix(double pitch, double height, double radius, double angle, bool leftHanded);
	//int createBezier(OCCVertex *start, OCCVertex *end, std::vector<OCCStruct3d> points);
	//int createSpline(OCCVertex *start, OCCVertex *end, std::vector<OCCStruct3d> points,double tolerance);
	//int createNURBS(OCCVertex *start, OCCVertex *end, std::vector<OCCStruct3d> points,  DVec knots, DVec weights, IVec mult);

	bool canSetShape(const TopoDS_Shape& shape) const {
		return shape.ShapeType() == TopAbs_EDGE;
	}
	virtual const TopoDS_Shape& shape() const { return m_edge; }
	virtual const TopoDS_Edge&  edge() const { return m_edge; }
	virtual void setShape(const TopoDS_Shape& shape) { m_edge = TopoDS::Edge(shape); }

	
	static Handle<v8::Value> createLine(const v8::Arguments& args); 
	static Handle<v8::Value> createCircle(const v8::Arguments& args);
	static Handle<v8::Value> createArc3P(const v8::Arguments& args);

   static void Init(Handle<Object> target);
   static Handle<Value> New(const Arguments& args);
   static Persistent<FunctionTemplate> constructor;
};