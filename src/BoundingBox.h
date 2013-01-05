#pragma once

#include "Shape.h"
#include "Point3Wrap.h"

#include <limits>

const double qNaN = std::numeric_limits<double>::quiet_NaN();

class BoundingBox : public node::ObjectWrap {
    Bnd_Box m_box;
public:
    typedef class BoundingBox _ThisType;


    BoundingBox()
    {}

    const gp_XYZ nearPt() const {

        if (m_box.IsVoid()) {
            return gp_XYZ(qNaN,qNaN,qNaN);
        }
        Standard_Real aXmin,aYmin,aZmin;
        Standard_Real aXmax,aYmax,aZmax;
        m_box.Get(aXmin,aYmin,aZmin,aXmax,aYmax,aZmax);
        return gp_XYZ(aXmin,aYmin,aZmin);
    }

    const gp_XYZ farPt() const {
        if (m_box.IsVoid()) {
            return gp_XYZ(-qNaN,-qNaN,-qNaN);
        }
        Standard_Real aXmin,aYmin,aZmin;
        Standard_Real aXmax,aYmax,aZmax;
        m_box.Get(aXmin,aYmin,aZmin,aXmax,aYmax,aZmax);
        return gp_XYZ(aXmax,aYmax,aZmax);
    }

    bool isVoid() {
        return m_box.IsVoid() ? true : false ;
    }

    TEAROFF_POINT(BoundingBox,nearPt,Point3Wrap,gp_XYZ);
    TEAROFF_POINT(BoundingBox,farPt,Point3Wrap,gp_XYZ);

    static Handle<v8::Value> addPoint(const v8::Arguments& args);
    static Handle<v8::Value> isOut(const v8::Arguments& args);


    static Persistent<FunctionTemplate> constructor;
    static Handle<Object> NewInstance(const gp_Pnt& near,const gp_Pnt& far);
    static Handle<Object> NewInstance(const Bnd_Box& box);

    static Handle<Value> New(const v8::Arguments& args);
    static void Init(Handle<Object> target);

protected:
	static void Update(BoundingBox* pThis,const v8::Arguments& args);
};