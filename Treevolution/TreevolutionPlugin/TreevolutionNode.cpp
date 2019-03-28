#include "TreevolutionNode.h"

MTypeId TreevolutionNode::id(0x80000);
// GUI variables

void* TreevolutionNode::creator()
{
    return new TreevolutionNode;
}

MStatus TreevolutionNode::initialize()
{
    // attributes for each GUI var
    /*MFnNumericAttribute stepAttr;
    MFnNumericAttribute angAttr;
    MFnTypedAttribute gramAttr;
    MFnUnitAttribute timeAttr;
    MFnTypedAttribute geomAttr;*/

    MStatus returnStatus;

    // create attributes
    /*LSystemNode::stepSize = stepAttr.create("step", "st",
        MFnNumericData::kFloat, 1.0,
        &returnStatus);

    LSystemNode::angle = angAttr.create("angle", "an",
        MFnNumericData::kFloat, 30.0,
        &returnStatus);

    LSystemNode::grammar = gramAttr.create("grammar", "gr",
        MFnData::kString,
        &returnStatus);

    LSystemNode::time = timeAttr.create("time", "tm",
        MFnUnitAttribute::kTime,
        0.0, &returnStatus);

    LSystemNode::outputGeom = geomAttr.create("outputGeom", "out",
        MFnData::kMesh,
        &returnStatus);*/

    // add attributes
    /*returnStatus = addAttribute(LSystemNode::stepSize);
    returnStatus = addAttribute(LSystemNode::angle);
    returnStatus = addAttribute(LSystemNode::grammar);
    returnStatus = addAttribute(LSystemNode::time);
    returnStatus = addAttribute(LSystemNode::outputGeom);*/

    // attribute affects
    /*returnStatus = attributeAffects(LSystemNode::stepSize,
        LSystemNode::outputGeom);
    returnStatus = attributeAffects(LSystemNode::angle,
        LSystemNode::outputGeom);
    returnStatus = attributeAffects(LSystemNode::grammar,
        LSystemNode::outputGeom);
    returnStatus = attributeAffects(LSystemNode::time,
        LSystemNode::outputGeom);*/

    return MS::kSuccess;
}

MStatus TreevolutionNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus returnStatus;

    /*if (plug == outputGeom) {

        MDataHandle stepData = data.inputValue(stepSize, &returnStatus);
        MDataHandle angData = data.inputValue(angle, &returnStatus);
        MDataHandle gramData = data.inputValue(grammar, &returnStatus);
        MString grammar = gramData.asString();

        MDataHandle timeData = data.inputValue(time, &returnStatus);
        MTime time = timeData.asTime();

        MDataHandle outputHandle = data.outputValue(outputGeom, &returnStatus);

        MFnMeshData dataCreator;
        MObject newOutputData = dataCreator.create(&returnStatus);

        createMesh(stepData.asFloat(), angData.asFloat(), grammar, time, newOutputData, returnStatus);

        outputHandle.set(newOutputData);
        data.setClean(plug);
    }
    else
        return MS::kUnknownParameter;*/

    return MS::kSuccess;
}

//MObject TreevolutionNode::createMesh(const float step, const float angle, const MString& gram, const MTime& time, MObject& outData, MStatus& stat)
//{
    // DO MAIN
    
    /*LSystem sys;
    sys.setDefaultStep(step);
    sys.setDefaultAngle(angle);
    sys.loadProgramFromString(gram.asChar());

    std::vector<LSystem::Branch> branches;
    std::vector<LSystem::Geometry> models;
    sys.process(time.value(), branches, models);

    MPointArray points;
    MIntArray faceCounts;
    MIntArray faceConnects;
    for (LSystem::Branch b : branches)
    {
        CylinderMesh cyl(MPoint(b.first[0], b.first[1], b.first[2]), MPoint(b.second[0], b.second[1], b.second[2]));
        cyl.appendToMesh(points, faceCounts, faceConnects);
    }

    MFnMesh	meshFS;
    return meshFS.create(points.length(), faceCounts.length(),
        points, faceCounts, faceConnects, outData, &stat);*/
//}