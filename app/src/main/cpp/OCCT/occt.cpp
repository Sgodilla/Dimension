//
// Created by sgodi on 8/26/2021.
//

#include "glm.h"
#include "occt.h"
#include <map>




void Mesh(TopoDS_Shape aShape, IMeshTools_Parameters aMeshParams, std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
    vertices.clear();
    indices.clear();

    BRepTools::Clean(aShape);
    BRepMesh_IncrementalMesh aMesher(aShape, aMeshParams);

    TopExp_Explorer aExpFace, aExpEdge;
    TopLoc_Location aLocation;
    int count = 0;
    int indexCount = 0;

    // Iterate through object, faces, and points
    for (aExpFace.Init(aShape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
    {
        count++;

        TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());

        Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace, aLocation);
        Handle(Geom_Surface) surf = BRep_Tool::Surface(aFace);

        if (!aTr.IsNull())
        {
            int numNodes = aTr->NbNodes();
            int numTriangles = aTr->NbTriangles();

            gp_Pnt aPnt;

            // Get Triangles
            int v1, v2, v3;
            for (int i = 1; i < numTriangles + 1; i++)
            {
                aTr->Triangle(i).Get(v1, v2, v3);
                indices.push_back(indexCount); indexCount++;
                indices.push_back(indexCount); indexCount++;
                indices.push_back(indexCount); indexCount++;

                pushVertices(aFace, vertices, aTr, surf, v1);
                pushVertices(aFace, vertices, aTr, surf, v2);
                pushVertices(aFace, vertices, aTr, surf, v3);
            }
        }
    }
}

void pushVertices(TopoDS_Face aFace, std::vector<float>& vertices, Handle(Poly_Triangulation) aTr, Handle(Geom_Surface) surf, int v)
{
    gp_Pnt aPnt = aTr->Node(v);
    vertices.push_back(aPnt.X());
    vertices.push_back(aPnt.Y());
    vertices.push_back(aPnt.Z());

    gp_Pnt2d uvPnt = gp_Pnt2d();
    gp_Dir aNorm = gp_Dir();

    try
    {
        uvPnt = aTr->UVNode(v);
        GeomLProp_SLProps props = GeomLProp_SLProps(surf, uvPnt.X(), uvPnt.Y(), 1, 0.01);
        aNorm = props.Normal();
    }
    catch (Standard_Failure const& theFailure)
    {
        std::cout << "This property is undefined! " << theFailure.GetMessageString() << std::endl;
    }

    vertices.push_back(uvPnt.X());
    vertices.push_back(uvPnt.Y());

    if (aFace.Orientation() == TopAbs_REVERSED)
        aNorm.Reverse();

    vertices.push_back(aNorm.X());
    vertices.push_back(aNorm.Y());
    vertices.push_back(aNorm.Z());
}

void Mesh(TopoDS_Shape aShape, IMeshTools_Parameters aMeshParams, std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec4 color)
{
    BRepTools::Clean(aShape);
    BRepMesh_IncrementalMesh aMesher(aShape, aMeshParams);

    TopExp_Explorer aExpFace, aExpEdge;
    TopLoc_Location aLocation;

    // Iterate through object, faces, and points
    for (aExpFace.Init(aShape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
    {
        TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());

        Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace, aLocation);
        Handle(Geom_Surface) surf = BRep_Tool::Surface(aFace);

        if (!aTr.IsNull())
        {
            int numNodes = aTr->NbNodes();
            int numTriangles = aTr->NbTriangles();

            gp_Pnt aPnt;

            // Get Triangles
            int v1, v2, v3;
            for (int i = 1; i < numTriangles + 1; i++)
            {
                aTr->Triangle(i).Get(v1, v2, v3);
                indices.push_back(indices.size());
                indices.push_back(indices.size());
                indices.push_back(indices.size());

                pushVertices(aFace, vertices, aTr, surf, v1, color);
                pushVertices(aFace, vertices, aTr, surf, v2, color);
                pushVertices(aFace, vertices, aTr, surf, v3, color);
            }
        }
    }
}

void Mesh(TopoDS_Shape aShape, IMeshTools_Parameters aMeshParams, std::vector<float>& vertices, std::vector<unsigned int>& indices, glm::vec4 color, glm::vec3 position)
{
    BRepTools::Clean(aShape);
    BRepMesh_IncrementalMesh aMesher(aShape, aMeshParams);

    TopExp_Explorer aExpFace, aExpEdge;
    TopLoc_Location aLocation;

    // Iterate through object, faces, and points
    for (aExpFace.Init(aShape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
    {
        TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());

        Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace, aLocation);
        Handle(Geom_Surface) surf = BRep_Tool::Surface(aFace);

        if (!aTr.IsNull())
        {
            int numNodes = aTr->NbNodes();
            int numTriangles = aTr->NbTriangles();

            gp_Pnt aPnt;

            // Get Triangles
            int v1, v2, v3;
            for (int i = 1; i < numTriangles + 1; i++)
            {
                aTr->Triangle(i).Get(v1, v2, v3);
                indices.push_back(indices.size());
                indices.push_back(indices.size());
                indices.push_back(indices.size());

                pushVertices(aFace, vertices, aTr, surf, v1, color, position);
                pushVertices(aFace, vertices, aTr, surf, v2, color, position);
                pushVertices(aFace, vertices, aTr, surf, v3, color, position);
            }
        }
    }
}

void Mesh(TopoDS_Shape aShape, IMeshTools_Parameters aMeshParams, std::vector<GLfloat> &vertices, std::vector<GLfloat> &normals, std::vector<GLfloat>& uvs, std::vector<GLushort>& indices)
{
    BRepTools::Clean(aShape);
    BRepMesh_IncrementalMesh aMesher(aShape, aMeshParams);

    TopExp_Explorer aExpFace, aExpEdge;
    TopLoc_Location aLocation;

    // Iterate through object, faces, and points
    for (aExpFace.Init(aShape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
    {
        TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());

        Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace, aLocation);
        Handle(Geom_Surface) surf = BRep_Tool::Surface(aFace);

        if (!aTr.IsNull())
        {
            int numNodes = aTr->NbNodes();
            int numTriangles = aTr->NbTriangles();

            gp_Pnt aPnt;

            // Get Triangles
            int v1, v2, v3;
            for (int i = 1; i < numTriangles + 1; i++)
            {
                aTr->Triangle(i).Get(v1, v2, v3);
                indices.push_back(indices.size());
                indices.push_back(indices.size());
                indices.push_back(indices.size());

                pushVertices(aFace, aTr, surf, v1, vertices, normals, uvs, indices);
                pushVertices(aFace, aTr, surf, v2, vertices, normals, uvs, indices);
                pushVertices(aFace, aTr, surf, v3, vertices, normals, uvs, indices);
            }
        }
    }
}

void pushVertices(TopoDS_Face aFace, std::vector<float>& vertices, Handle(Poly_Triangulation) aTr, Handle(Geom_Surface) surf, int v, glm::vec4 color, glm::vec3 position)
{
    gp_Pnt aPnt = aTr->Node(v);
    vertices.push_back(aPnt.X() + position.x);
    vertices.push_back(aPnt.Y() + position.y);
    vertices.push_back(aPnt.Z() + position.z);

    gp_Pnt2d uvPnt = gp_Pnt2d();
    gp_Dir aNorm = gp_Dir();

    try
    {
        uvPnt = aTr->UVNode(v);
        GeomLProp_SLProps props = GeomLProp_SLProps(surf, uvPnt.X(), uvPnt.Y(), 1, 0.01);
        aNorm = props.Normal();
    }
    catch (Standard_Failure const& theFailure)
    {
        std::cout << "This property is undefined! " << theFailure.GetMessageString() << std::endl;
    }

    vertices.push_back(uvPnt.X());
    vertices.push_back(uvPnt.Y());

    if (aFace.Orientation() == TopAbs_REVERSED)
        aNorm.Reverse();

    vertices.push_back(aNorm.X());
    vertices.push_back(aNorm.Y());
    vertices.push_back(aNorm.Z());

    vertices.push_back(color.r);
    vertices.push_back(color.g);
    vertices.push_back(color.b);
    vertices.push_back(color.a);
}

void pushVertices(TopoDS_Face aFace, std::vector<float>& vertices, Handle(Poly_Triangulation) aTr, Handle(Geom_Surface) surf, int v, glm::vec4 color)
{
    gp_Pnt aPnt = aTr->Node(v);
    vertices.push_back(aPnt.X());
    vertices.push_back(aPnt.Y());
    vertices.push_back(aPnt.Z());

    gp_Pnt2d uvPnt = gp_Pnt2d();
    gp_Dir aNorm = gp_Dir();

    try
    {
        uvPnt = aTr->UVNode(v);
        GeomLProp_SLProps props = GeomLProp_SLProps(surf, uvPnt.X(), uvPnt.Y(), 1, 0.01);
        aNorm = props.Normal();
    }
    catch (Standard_Failure const& theFailure)
    {
        std::cout << "This property is undefined! " << theFailure.GetMessageString() << std::endl;
    }


    vertices.push_back(uvPnt.X());
    vertices.push_back(uvPnt.Y());

    if (aFace.Orientation() == TopAbs_REVERSED)
        aNorm.Reverse();

    vertices.push_back(aNorm.X());
    vertices.push_back(aNorm.Y());
    vertices.push_back(aNorm.Z());

    vertices.push_back(color.r);
    vertices.push_back(color.g);
    vertices.push_back(color.b);
    vertices.push_back(color.a);
}

void pushVertices(TopoDS_Face aFace, Handle(Poly_Triangulation) aTr, Handle(Geom_Surface) surf, int v, std::vector<GLfloat>& vertices, std::vector<GLfloat>& normals, std::vector<GLfloat>& uvs, std::vector<GLushort>& indices)
{
    gp_Pnt aPnt = aTr->Node(v);
    vertices.push_back(aPnt.X());
    vertices.push_back(aPnt.Y());
    vertices.push_back(aPnt.Z());

    gp_Pnt2d uvPnt = gp_Pnt2d();
    gp_Dir aNorm = gp_Dir();

    try
    {
        uvPnt = aTr->UVNode(v);
        GeomLProp_SLProps props = GeomLProp_SLProps(surf, uvPnt.X(), uvPnt.Y(), 1, 0.01);
        aNorm = props.Normal();
    }
    catch (Standard_Failure const& theFailure)
    {
        std::cout << "This property is undefined! " << theFailure.GetMessageString() << std::endl;
    }

    uvs.push_back(uvPnt.X());
    uvs.push_back(uvPnt.Y());

    if (aFace.Orientation() == TopAbs_REVERSED)
        aNorm.Reverse();

    normals.push_back(aNorm.Y());
    normals.push_back(aNorm.X());
    normals.push_back(aNorm.Z());
}

TopoDS_Shape MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight,
                        const Standard_Real myThickness)
{
    // Profile : Define Support Points
    gp_Pnt aPnt1(-myWidth / 2., 0, 0);
    gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
    gp_Pnt aPnt3(0, -myThickness / 2., 0);
    gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
    gp_Pnt aPnt5(myWidth / 2., 0, 0);

    // Profile : Define the Geometry
    Handle(Geom_TrimmedCurve) anArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
    Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
    Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

    // Profile : Define the Topology
    TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
    TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
    TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
    TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);

    // Complete Profile
    gp_Ax1 xAxis = gp::OX();
    gp_Trsf aTrsf;

    aTrsf.SetMirror(xAxis);
    BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
    TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
    TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);

    BRepBuilderAPI_MakeWire mkWire;
    mkWire.Add(aWire);
    mkWire.Add(aMirroredWire);
    TopoDS_Wire myWireProfile = mkWire.Wire();

    // Body : Prism the Profile
    TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
    gp_Vec aPrismVec(0, 0, myHeight);
    TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

    // Body : Apply Fillets
    BRepFilletAPI_MakeFillet mkFillet(myBody);
    TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
    while (anEdgeExplorer.More()) {
        TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
        //Add edge to fillet algorithm
        mkFillet.Add(myThickness / 12., anEdge);
        anEdgeExplorer.Next();
    }

    myBody = mkFillet.Shape();

    // Body : Add the Neck
    gp_Pnt neckLocation(0, 0, myHeight);
    gp_Dir neckAxis = gp::DZ();
    gp_Ax2 neckAx2(neckLocation, neckAxis);

    Standard_Real myNeckRadius = myThickness / 4.;
    Standard_Real myNeckHeight = myHeight / 10.;

    BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
    TopoDS_Shape myNeck = MKCylinder.Shape();

    myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

    // Body : Create a Hollowed Solid
    TopoDS_Face   faceToRemove;
    Standard_Real zMax = -1;

    for (TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next()) {
        TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
        // Check if <aFace> is the top face of the bottle's neck
        Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
        if (aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
            Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);
            gp_Pnt aPnt = aPlane->Location();
            Standard_Real aZ = aPnt.Z();
            if (aZ > zMax) {
                zMax = aZ;
                faceToRemove = aFace;
            }
        }
    }

    TopTools_ListOfShape facesToRemove;
    facesToRemove.Append(faceToRemove);
    BRepOffsetAPI_MakeThickSolid BodyMaker;
    BodyMaker.MakeThickSolidByJoin(myBody, facesToRemove, -myThickness / 50, 1.e-3);
    myBody = BodyMaker.Shape();
    // Threading : Create Surfaces
    Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99);
    Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);

    // Threading : Define 2D Curves
    gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
    gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
    gp_Ax2d anAx2d(aPnt, aDir);

    Standard_Real aMajor = 2. * M_PI;
    Standard_Real aMinor = myNeckHeight / 10;

    Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
    Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
    Handle(Geom2d_TrimmedCurve) anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
    Handle(Geom2d_TrimmedCurve) anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
    gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
    gp_Pnt2d anEllipsePnt2 = anEllipse1->Value(M_PI);

    Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);
    // Threading : Build Edges and Wires
    TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
    TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
    TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
    TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
    TopoDS_Wire threadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
    TopoDS_Wire threadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
    BRepLib::BuildCurves3d(threadingWire1);
    BRepLib::BuildCurves3d(threadingWire2);

    // Create Threading
    BRepOffsetAPI_ThruSections aTool(Standard_True);
    aTool.AddWire(threadingWire1);
    aTool.AddWire(threadingWire2);
    aTool.CheckCompatibility(Standard_False);

    TopoDS_Shape myThreading = aTool.Shape();

    // Building the Resulting Compound
    TopoDS_Compound aRes;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound(aRes);
    aBuilder.Add(aRes, myBody);
    aBuilder.Add(aRes, myThreading);

    return aRes;
}