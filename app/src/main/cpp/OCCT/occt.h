//
// Created by sgodi on 8/26/2021.
//

#ifndef HELLO_AR_C_OCCT_H
#define HELLO_AR_C_OCCT_H

// OCC
#include "BRepPrimAPI_MakeCylinder.hxx"
#include "BRepTools.hxx"
#include "BRepMesh_IncrementalMesh.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "BOPTools_AlgoTools3D.hxx"
#include "BRep_Tool.hxx"

#include "BRepAlgoAPI_Fuse.hxx"

#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "BRepBuilderAPI_Transform.hxx"

#include "BRepFilletAPI_MakeFillet.hxx"

#include "BRepLib.hxx"

#include "BRepOffsetAPI_MakeThickSolid.hxx"
#include "BRepOffsetAPI_ThruSections.hxx"

#include "BRepPrimAPI_MakeCylinder.hxx"
#include "BRepPrimAPI_MakePrism.hxx"

#include "GC_MakeArcOfCircle.hxx"
#include "GC_MakeSegment.hxx"

#include "GCE2d_MakeSegment.hxx"

#include "gp.hxx"
#include "gp_Ax1.hxx"
#include "gp_Ax2.hxx"
#include "gp_Ax2d.hxx"
#include "gp_Dir.hxx"
#include "gp_Dir2d.hxx"
#include "gp_Pnt.hxx"
#include "gp_Pnt2d.hxx"
#include "gp_Trsf.hxx"
#include "gp_Vec.hxx"
#include "gp_Quaternion.hxx"

#include "Geom_CylindricalSurface.hxx"
#include "Geom_Plane.hxx"
#include "Geom_Surface.hxx"
#include "Geom_TrimmedCurve.hxx"
#include "GeomLProp_SLProps.hxx"

#include "Geom2d_Ellipse.hxx"
#include "Geom2d_TrimmedCurve.hxx"

# include"BRep_Builder.hxx"
#include "BRepBuilderAPI_Transform.hxx"

# include"TopTools_HSequenceOfShape.hxx"
# include"STEPControl_Writer.hxx"
# include"STEPControl_Reader.hxx"
# include"StepData_StepModel.hxx"
# include"TopoDS.hxx"
# include"TopoDS_Shape.hxx"
# include"TopoDS_Shell.hxx"
# include"TopoDS_Solid.hxx"
# include"TopoDS_Compound.hxx"
# include"TopExp_Explorer.hxx"

#include "Quantity_Color.hxx"
#include "Quantity_ColorRGBA.hxx"

#include "STEPCAFControl_Reader.hxx"
#include "TDocStd_Document.hxx"
#include "XCAFDoc_DocumentTool.hxx"
#include "XCAFDoc_ShapeTool.hxx"
#include "XCAFDoc_ColorTool.hxx"
#include "XCAFDoc_Centroid.hxx"

#include "TDataStd_Name.hxx"
#include "TCollection_ExtendedString.hxx"
#include "TDF_ChildIterator.hxx"
#include "util.h"

void Mesh(TopoDS_Shape aShape, IMeshTools_Parameters aMeshParams, std::vector<float>& vertices, std::vector<unsigned int>& indices);

void Mesh(TopoDS_Shape aShape, IMeshTools_Parameters aMeshParams, std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec4 color);

void Mesh(TopoDS_Shape aShape, IMeshTools_Parameters aMeshParams, std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec4 color, glm::vec3 position);

void Mesh(TopoDS_Shape aShape, IMeshTools_Parameters aMeshParams, std::vector<GLfloat> &vertices, std::vector<GLfloat> &normals, std::vector<GLfloat> &uvs, std::vector<GLushort> &indices);

void pushVertices(TopoDS_Face aFace, std::vector<float>& vertices, Handle(Poly_Triangulation) aTr, Handle(Geom_Surface) surf, int v, glm::vec4 color, glm::vec3 position);

void pushVertices(TopoDS_Face aFace, std::vector<float>& vertices, Handle(Poly_Triangulation) aTr, Handle(Geom_Surface) surf, int v, glm::vec4 color);

void pushVertices(TopoDS_Face aFace, std::vector<float>& vertices, Handle(Poly_Triangulation) aTr, Handle(Geom_Surface) surf, int v);

void pushVertices(TopoDS_Face aFace, Handle(Poly_Triangulation) aTr, Handle(Geom_Surface) surf, int v, std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& uvs, std::vector<GLushort>& indices);

TopoDS_Shape MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight, const Standard_Real myThickness);

#endif //HELLO_AR_C_OCCT_H
