#pragma once

//#include "LSystem.h"
//#include "cylinder.h"

#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MPoint.h>
#include <maya/MPlug.h>
#include <maya/MObject.h>
#include <maya/MPxNode.h>
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>

class TreevolutionNode : public MPxNode
{
public:
    static MTypeId id;
    static MObject meshString;
    static MObject numPreview;
    static MObject outputMesh;
    // TODO: GUI variables

    TreevolutionNode() {}
    virtual ~TreevolutionNode() {}
    static void* creator();
    static MStatus initialize();
    virtual MStatus compute(const MPlug& plug, MDataBlock& data);

protected:
    MObject createMesh(MObject& outData, const MString& meshStr, const int nPrev, MStatus& stat);
};
