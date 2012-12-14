// OpenCASCADE
#pragma once

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepLib.hxx>
#include <BRepMesh.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>

#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <ShapeFix_Solid.hxx>

#include <StlAPI_Writer.hxx>
#include <StlAPI_Reader.hxx>

#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Elips.hxx>
#include <gp_Sphere.hxx>
#include <Geom_BezierCurve.hxx>

#include <GCPnts_UniformDeflection.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>

#include <Poly_Triangulation.hxx>
#include <Poly_Connect.hxx>

#include <TColgp_Array1OfDir.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <StdPrs_ToolShadedShape.hxx>

#include <FSD_BinaryFile.hxx>

#include <GC_MakeSegment.hxx>

#include <Storage_Data.hxx>
#include <ShapeSchema.hxx>

#include <MgtBRep.hxx>

#include <Storage_HSeqOfRoot.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Storage_Root.hxx>
#include <BRepCheck_Analyzer.hxx>

#include <TopExp.hxx>
/*
#include <PTColStd_PersistentTransientMap.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
*/
#undef Handle
