#pragma once
#include "NodeV8.h"
#include "OCC.h"
#include "Util.h"

#include "Point3Wrap.h"



class Transformation : public node::ObjectWrap {
public:
    typedef class Transformation _ThisType ;
    Transformation()
    {}
    static Handle<Value> makeTranslation(const Arguments& args);
    static Handle<Value> makePlaneMirror(const Arguments& args);
    static Handle<Value> makeAxisMirror(const Arguments& args);
    static Handle<Value> makeScale(const Arguments& args);
    static Handle<Value> makeRotation(const Arguments& args);

    double scaleFactor() {
        return m_trsf.ScaleFactor();
    }

    const gp_XYZ translationPart() const  {
        return m_trsf.TranslationPart();
    }

    TEAROFF_POINT(Transformation,translationPart,Point3Wrap,gp_XYZ);

    gp_Trsf m_trsf;




    // Methods exposed to JavaScripts
    static void Init(Handle<Object> target);
    static Handle<Value> NewInstance(const Arguments& args);
    static Handle<Value> New(const Arguments& args);

    static Persistent<FunctionTemplate> constructor;

private:
    Transformation(const Transformation&);
    void operator=(const Transformation&);
};