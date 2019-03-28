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

class TreevolutionNode : public MPxNode
{
public:
    static MTypeId id;

    // TODO: GUI variables

    TreevolutionNode() {}
    virtual ~TreevolutionNode() {}
    static void* creator();
    static MStatus initialize();
    virtual MStatus compute(const MPlug& plug, MDataBlock& data);

//protected:
    //MObject createMesh(const float step, const float angle, const MString& gram, const MTime& time, MObject& outData, MStatus& stat);
};
