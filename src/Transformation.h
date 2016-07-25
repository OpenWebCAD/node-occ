#pragma once
#include "NodeV8.h"
#include "OCC.h"
#include "Util.h"

#include "Point3Wrap.h"



class Transformation : public Nan::ObjectWrap {
public:
    typedef class Transformation _ThisType ;
    Transformation()
    {}
    static NAN_METHOD(makeTranslation);
    static NAN_METHOD(makePlaneMirror);
    static NAN_METHOD(makeAxisMirror);
    static NAN_METHOD(makeScale);
    static NAN_METHOD(makeRotation);

    double scaleFactor() {
        return m_trsf.ScaleFactor();
    }

    const gp_XYZ translationPart() const  {
        return m_trsf.TranslationPart();
    }

    TEAROFF_POINT(Transformation,translationPart,Point3Wrap,gp_XYZ);

    gp_Trsf m_trsf;




    // Methods exposed to JavaScripts
    static void Init(v8::Handle<v8::Object> target);
    static NAN_METHOD(NewInstance);
    static NAN_METHOD(New);

    static Nan::Persistent<v8::FunctionTemplate> _template;

private:
    Transformation(const Transformation&);
    void operator=(const Transformation&);
};