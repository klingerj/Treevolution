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
    static MObject meshLeafString;
    static MObject populationSize;
    static MObject numGen;
    static MObject numPreview;
    static MObject fitnessEvalType;
    static MObject tgtVolString;
    static MObject outputMesh;

    TreevolutionNode() {}
    virtual ~TreevolutionNode() {}
    static void* creator();
    static MStatus initialize();
    virtual MStatus compute(const MPlug& plug, MDataBlock& data);

protected:
    MObject createMesh(MObject& outData, const MString& meshStr, const MString& leafStr, const int poplSize, const int nGen, const int nPrev, const MString& fitStr,
        const MString& tgtStr, MStatus& stat);
};
