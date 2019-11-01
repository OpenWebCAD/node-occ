#pragma once

#include "Shape.h"
#include "Point3Wrap.h"

#include <limits>

const double qNaN = std::numeric_limits<double>::quiet_NaN();

class BoundingBox : public Nan::ObjectWrap {
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
    TEAROFF_POINT(BoundingBox,farPt, Point3Wrap,gp_XYZ);

    static NAN_METHOD(addPoint);
    static NAN_METHOD(isOut);


   virtual void InitNew(_NAN_METHOD_ARGS);

    static Nan::Persistent<v8::FunctionTemplate> _template;

    static v8::Local<v8::Value> NewInstance(const gp_Pnt& near,const gp_Pnt& far);
    static v8::Local<v8::Value> NewInstance(const Bnd_Box& box);

    static NAN_METHOD(New);
    static void Init(v8::Local<v8::Object> target);

protected:
	static void Update(BoundingBox* pThis,_NAN_METHOD_ARGS);
};