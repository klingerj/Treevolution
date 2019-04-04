#include "TreevolutionNode.h"

#include "LSystem.h"
#include "Scene/Mesh.h"
#include "GeneticAlgorithms/TreeStructure.h"

MTypeId TreevolutionNode::id(0x80000);
MObject TreevolutionNode::meshString;
MObject TreevolutionNode::outputMesh;
// more GUI variables

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
    MFnUnitAttribute timeAttr;*/
    MFnTypedAttribute inMeshAttr;
    MFnTypedAttribute meshAttr;

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
        0.0, &returnStatus);*/

    TreevolutionNode::meshString = inMeshAttr.create("inMesh", "in",
        MFnData::kString,
        &returnStatus);

    TreevolutionNode::outputMesh = meshAttr.create("outputMesh", "out",
        MFnData::kMesh,
        &returnStatus);

    // add attributes
    /*returnStatus = addAttribute(LSystemNode::stepSize);
    returnStatus = addAttribute(LSystemNode::angle);
    returnStatus = addAttribute(LSystemNode::grammar);
    returnStatus = addAttribute(LSystemNode::time);*/
    returnStatus = addAttribute(TreevolutionNode::meshString);
    returnStatus = addAttribute(TreevolutionNode::outputMesh);

    // attribute affects
    /*returnStatus = attributeAffects(LSystemNode::stepSize,
        LSystemNode::outputGeom);
    returnStatus = attributeAffects(LSystemNode::angle,
        LSystemNode::outputGeom);
    returnStatus = attributeAffects(LSystemNode::grammar,
        LSystemNode::outputGeom);
    returnStatus = attributeAffects(LSystemNode::time,
        LSystemNode::outputGeom);*/
    returnStatus = attributeAffects(TreevolutionNode::meshString,
        TreevolutionNode::outputMesh);

    MGlobal::displayInfo(MString("setup complete"));
    return MS::kSuccess;
}

MStatus TreevolutionNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus returnStatus;

    MGlobal::displayInfo(MString("made it"));
    if (plug == outputMesh) {

        //MGlobal::displayInfo(MString("HELLO... BEB?"));
        /*MDataHandle stepData = data.inputValue(stepSize, &returnStatus);
        MDataHandle angData = data.inputValue(angle, &returnStatus);
        MDataHandle gramData = data.inputValue(grammar, &returnStatus);
        MString grammar = gramData.asString();

        MDataHandle timeData = data.inputValue(time, &returnStatus);
        MTime time = timeData.asTime();*/

        MDataHandle inMeshData = data.inputValue(meshString, &returnStatus);
        MString meshStr = inMeshData.asString();
        MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);

        MFnMeshData dataCreator;
        MObject newOutputData = dataCreator.create(&returnStatus);

        createMesh(newOutputData, meshStr, returnStatus);

        outputHandle.set(newOutputData);
        data.setClean(plug);
    }
    else
        return MS::kUnknownParameter;

    return MS::kSuccess;
}

MObject TreevolutionNode::createMesh(MObject& outData, const MString& meshStr, MStatus& stat)
{
    // DO MAIN
    MGlobal::displayInfo(MString("IN CREATE"));
    MGlobal::displayInfo(meshStr);
    
    // Load base branch model
    Mesh branchMesh = Mesh();
    branchMesh.LoadFromFile(meshStr.asChar());

    // Create L-system
    LSystem sys;
    sys.setDefaultStep(0.1f);
    sys.setDefaultAngle(30.0f);
    sys.loadProgramFromString("F\nF->F[+F][-F]"); //taken from simple1.txt
    std::string iteratedStr = sys.getIteration(1);

    TreeStructure theTree = TreeStructure(1, iteratedStr, 0.0f, 90.0f, 1.0f, 3.0f);
    //TreeStructure theTree2 = TreeStructure(2, iteratedStr, 0.0f, 90.0f, 1.0f, 3.0f);
    //theTree.Crossover(&theTree2);
    Mesh treeMesh = theTree.GetTreeMesh(branchMesh);
    //Mesh treeMesh2 = theTree2.GetTreeMesh(branchMesh);

    MPointArray points;
    MIntArray faceCounts;
    MIntArray faceConnects;
    int start = 0;
    for (Triangle t : treeMesh.GetTriangles())
    {
        std::vector<glm::vec3> pts = t.GetPoints();
        points.append(MPoint(pts[0][0], pts[0][1], pts[0][2]));
        points.append(MPoint(pts[1][0], pts[1][1], pts[1][2]));
        points.append(MPoint(pts[2][0], pts[2][1], pts[2][2]));
        
        faceCounts.append(3);

        for (int i = 0; i < 3; i++)
        {
            faceConnects.append(i + start);
        }
        start += 3;
    }

    MFnMesh	meshFS;
    return meshFS.create(points.length(), faceCounts.length(),
        points, faceCounts, faceConnects, outData, &stat);
}