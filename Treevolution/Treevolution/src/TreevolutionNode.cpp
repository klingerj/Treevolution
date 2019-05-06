#include "TreevolutionNode.h"

#include "LSystem.h"
#include "Scene/Mesh.h"
#include "Scene/Camera.h"
#include "GeneticAlgorithms/TreeStructure.h"
#include "GeneticAlgorithms/Fitness/FitnessEvalMethod.h"
#include <glm/gtc/matrix_transform.hpp>

MTypeId TreevolutionNode::id(0x80000);
MObject TreevolutionNode::meshString;
MObject TreevolutionNode::meshLeafString;
MObject TreevolutionNode::populationSize;
MObject TreevolutionNode::numGen;
MObject TreevolutionNode::numPreview;
MObject TreevolutionNode::fitnessEvalType;
MObject TreevolutionNode::tgtVolString;
MObject TreevolutionNode::outputMesh;

void* TreevolutionNode::creator()
{
    return new TreevolutionNode;
}

MStatus TreevolutionNode::initialize()
{
    // attributes for each GUI var
    MFnTypedAttribute inMeshAttr;
    MFnTypedAttribute inLeafMeshAttr;
    MFnNumericAttribute popSizeAttr;
    MFnNumericAttribute nGenAttr;
    MFnNumericAttribute nPrevAttr;
    MFnTypedAttribute fitEvalAttr;
    MFnTypedAttribute tgtVolAttr;
    MFnTypedAttribute meshAttr;

    MStatus returnStatus;

    // create attributes
    TreevolutionNode::meshString = inMeshAttr.create("inMesh", "in",
        MFnData::kString,
        &returnStatus);

    TreevolutionNode::meshLeafString = inLeafMeshAttr.create("inLeafMesh", "il",
        MFnData::kString,
        &returnStatus);

    TreevolutionNode::populationSize = popSizeAttr.create("popSize", "ps",
        MFnNumericData::kInt, 1,
        &returnStatus);

    TreevolutionNode::numGen = nPrevAttr.create("numGen", "ng",
        MFnNumericData::kInt, 1,
        &returnStatus);

    TreevolutionNode::numPreview = nPrevAttr.create("numPreview", "np",
        MFnNumericData::kInt, 1,
        &returnStatus);

    TreevolutionNode::fitnessEvalType = fitEvalAttr.create("fitnessType", "ft",
        MFnData::kString,
        &returnStatus);

    TreevolutionNode::tgtVolString = inMeshAttr.create("tgtVolume", "tv",
        MFnData::kString,
        &returnStatus);

    TreevolutionNode::outputMesh = meshAttr.create("outputMesh", "out",
        MFnData::kMesh,
        &returnStatus);

    // add attributes
    returnStatus = addAttribute(TreevolutionNode::meshString);
    returnStatus = addAttribute(TreevolutionNode::meshLeafString);
    returnStatus = addAttribute(TreevolutionNode::populationSize);
    returnStatus = addAttribute(TreevolutionNode::numGen);
    returnStatus = addAttribute(TreevolutionNode::numPreview);
    returnStatus = addAttribute(TreevolutionNode::fitnessEvalType);
    returnStatus = addAttribute(TreevolutionNode::tgtVolString);
    returnStatus = addAttribute(TreevolutionNode::outputMesh);

    // attribute affects
    returnStatus = attributeAffects(TreevolutionNode::meshString,
        TreevolutionNode::outputMesh);
    returnStatus = attributeAffects(TreevolutionNode::meshLeafString,
        TreevolutionNode::outputMesh);
    returnStatus = attributeAffects(TreevolutionNode::populationSize,
        TreevolutionNode::outputMesh);
    returnStatus = attributeAffects(TreevolutionNode::numGen,
        TreevolutionNode::outputMesh);
    returnStatus = attributeAffects(TreevolutionNode::numPreview,
        TreevolutionNode::outputMesh);
    returnStatus = attributeAffects(TreevolutionNode::fitnessEvalType,
        TreevolutionNode::outputMesh);
    returnStatus = attributeAffects(TreevolutionNode::tgtVolString,
        TreevolutionNode::outputMesh);

    MGlobal::displayInfo(MString("setup complete"));
    return MS::kSuccess;
}

MStatus TreevolutionNode::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus returnStatus;

    if (plug == outputMesh) {

        MDataHandle inMeshData = data.inputValue(meshString, &returnStatus);
        MString meshStr = inMeshData.asString();
        MDataHandle inLeafMeshData = data.inputValue(meshLeafString, &returnStatus);
        MString leafStr = inLeafMeshData.asString();
        MDataHandle popData = data.inputValue(populationSize, &returnStatus);
        MDataHandle ngData = data.inputValue(numGen, &returnStatus);
        MDataHandle npData = data.inputValue(numPreview, &returnStatus);
        MDataHandle fitData = data.inputValue(fitnessEvalType, &returnStatus);
        MString fitnessStr = fitData.asString();
        MDataHandle tgtVolData = data.inputValue(tgtVolString, &returnStatus);
        MString tgtVolStr = tgtVolData.asString();
        MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);

        MFnMeshData dataCreator;
        MObject newOutputData = dataCreator.create(&returnStatus);

        createMesh(newOutputData, meshStr, leafStr, popData.asInt(), ngData.asInt(), npData.asInt(), fitnessStr, tgtVolStr, returnStatus);

        outputHandle.set(newOutputData);
        data.setClean(plug);
    }
    else
        return MS::kUnknownParameter;

    return MS::kSuccess;
}

MObject TreevolutionNode::createMesh(MObject& outData, const MString& meshStr, const MString& leafStr, const int poplSize, const int nGen, const int nPrev, const MString& fitStr,
    const MString& tgtStr, MStatus& stat)
{
    // DO MAIN
    
    // Load base branch model
    Mesh branchMesh = Mesh();
    branchMesh.LoadFromFile(meshStr.asChar());

    Mesh leafMesh = Mesh();
    leafMesh.LoadFromFile(leafStr.asChar());

    // Create L-system
    LSystem sys;
    sys.setDefaultStep(0.1f);
    sys.setDefaultAngle(30.0f);
    sys.loadProgramFromString("F\nF->FXXX[X+FXX]XXXX[XX-FXX]XXXX\nF->FXXXX\nF->FXXX[X+FXXXX]XXXX");

    std::vector<Mesh> treeMeshes;
    if (fitStr == MString("radioButton1"))
    {
        // Load reference model
        Mesh referenceMesh = Mesh();
        referenceMesh.LoadFromFile(tgtStr.asChar());
        
        // Fitness evaluation
        FitnessEvalMethod* eval = new VolumetricFitnessEval(0.35f);
        VolumetricFitnessEval* volumetricEval = dynamic_cast<VolumetricFitnessEval*>(eval);
        volumetricEval->SetGrid(referenceMesh, 0);

        std::vector<TreeStructure> population;
        const int popSize = poplSize;
        int elit = ceil(popSize / 5);
        if (elit % 2 == 1) { elit++; }
        const int elitism = elit; // must be even!!!!!!
        population.reserve(popSize * 2);
        for (int i = 0; i < popSize; ++i) {
            std::string iteratedStr = sys.getIteration(4, i);
            population.emplace_back(std::move(TreeStructure(i, iteratedStr, 0.0f, 90.0f, 0.25f, 3.0f)));
        }

        std::vector<TreeStructure> newPopulation;
        newPopulation.reserve(popSize * 2);

        const int numGenerations = nGen;
        for (int i = 0; i < numGenerations; ++i) {
            MString msg = MString("New Generation : ");
            msg += i;
            MGlobal::displayInfo(msg);
            // Compute fitness scores
            for (int j = 0; j < popSize; ++j) {
                Mesh treeMesh = population[j].GetTreeMesh(branchMesh, leafMesh); // Get the current pop member's mesh
                if (treeMesh.GetTriangles().size() == 0) {
                    population[j].fitnessScore = -9999999999;
                    continue;
                }
                else {
                    volumetricEval->SetGrid(treeMesh, 1); // set the grid points
                    population[j].fitnessScore = volumetricEval->Evaluate(); // get the evaluated score
                    // TODO: simplify the above to a single function call
                }
            }

            // Sort based on fitness
            std::sort(population.begin(), population.end(), [](TreeStructure& a, TreeStructure& b) { return a.fitnessScore > b.fitnessScore; });

            newPopulation.clear();

            // Elitism
            for (int e = 0; e < elitism; ++e) {
                newPopulation.push_back(population[e]);
            }

            // Crossover/Mutation
            for (int r = elitism; r - elitism < popSize - elitism; r += 2) {
                TreeStructure& par1 = population[r];
                TreeStructure& par2 = population[r + 1];
                int m1 = par1.Mutate(sys.getRules());
                int m2 = par2.Mutate(sys.getRules());
                if (m1 >= 3 && m2 >= 3) { // neither parent mutated
                    par1.Crossover(&par2);
                }
                newPopulation.push_back(par1);
                newPopulation.push_back(par2);
            }
            population = newPopulation;
        }

        // get tree meshes
        for (int i = 0; i < nPrev; ++i) {
            treeMeshes.push_back(population[i].GetTreeMesh(branchMesh, leafMesh));
        }
    }
    else if (fitStr == MString("radioButton2"))
    {
        Camera camera = Camera(glm::vec3(0, 5, 16), glm::vec3(0, 0, 0), 0.7853981634f, 1.0f, 0.01f, 100.0f);
        
        // Fitness evaluation
        FitnessEvalMethod* eval = new ImageFitnessEval();
        ImageFitnessEval* imageEval = dynamic_cast<ImageFitnessEval*>(eval);
        imageEval->SetRefImage(tgtStr.asChar());

        std::vector<TreeStructure> population;
        const int popSize = poplSize;
        int elit = ceil(popSize / 5);
        if (elit % 2 == 1) { elit++; }
        const int elitism = elit; // must be even!!!!!!
        population.reserve(popSize * 2);
        for (int i = 0; i < popSize; ++i) {
            std::string iteratedStr = sys.getIteration(4, i);
            population.emplace_back(std::move(TreeStructure(i, iteratedStr, 0.0f, 90.0f, 0.25f, 3.0f)));
        }

        std::vector<TreeStructure> newPopulation;
        newPopulation.reserve(popSize * 2);

        const int numGenerations = nGen;
        for (int i = 0; i < numGenerations; ++i) {
            MString msg = MString("New Generation : ");
            msg += i;
            MGlobal::displayInfo(msg);
            // Compute fitness scores
            for (int j = 0; j < popSize; ++j) {
                Mesh treeMesh = population[j].GetTreeMesh(branchMesh, leafMesh); // Get the current pop member's mesh
                if (treeMesh.GetTriangles().size() == 0) {
                    population[j].fitnessScore = -9999999999;
                    continue;
                }
                else {
                    imageEval->SetCurrImage(treeMesh, camera.GetViewProj(), glm::mat4(1.0f));
                    population[j].fitnessScore = imageEval->Evaluate(); // get the evaluated score
                }
            }

            // Sort based on fitness
            std::sort(population.begin(), population.end(), [](TreeStructure& a, TreeStructure& b) { return a.fitnessScore > b.fitnessScore; });

            newPopulation.clear();

            // Elitism
            for (int e = 0; e < elitism; ++e) {
                newPopulation.push_back(population[e]);
            }

            // Crossover/Mutation
            for (int r = elitism; r - elitism < popSize - elitism; r += 2) {
                TreeStructure& par1 = population[r];
                TreeStructure& par2 = population[r + 1];
                int m1 = par1.Mutate(sys.getRules());
                int m2 = par2.Mutate(sys.getRules());
                if (m1 >= 3 && m2 >= 3) { // neither parent mutated
                    par1.Crossover(&par2);
                }
                newPopulation.push_back(par1);
                newPopulation.push_back(par2);
            }
            population = newPopulation;
        }

        // get tree meshes
        for (int i = 0; i < nPrev; ++i) {
            treeMeshes.push_back(population[i].GetTreeMesh(branchMesh, leafMesh));
        }
    }

    // CREATING GEOMETRY
    MPointArray points;
    MIntArray faceCounts;
    MIntArray faceConnects;

    MString uniteCmd = MString("polyUnite -n result ");
    int start = 0;
    int count = 0;
    int treeCount = 0;
    for (Mesh treeMesh : treeMeshes)
    {
        const glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, float(treeCount*10)));
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

            pts[0] = glm::vec3(trans * glm::vec4(pts[0], 1.0));
            pts[1] = glm::vec3(trans * glm::vec4(pts[1], 1.0));
            pts[2] = glm::vec3(trans * glm::vec4(pts[2], 1.0));

            MString poly = MString("polyCreateFacet -n plg") + count + MString(" ");
            poly = poly + MString("-p ") + pts[0][0] + MString(" ") + pts[0][1] + MString(" ") + pts[0][2] + MString(" ");
            poly = poly + MString("-p ") + pts[1][0] + MString(" ") + pts[1][1] + MString(" ") + pts[1][2] + MString(" ");
            poly = poly + MString("-p ") + pts[2][0] + MString(" ") + pts[2][1] + MString(" ") + pts[2][2] + MString(";");

            uniteCmd = uniteCmd + MString("plg") + count + MString(" ");

            MGlobal::executeCommand(poly);
            count++;
        }
        treeCount++;
    }
    MGlobal::executeCommand(uniteCmd);

    MFnMesh	meshFS;
    return meshFS.create(points.length(), faceCounts.length(),
        points, faceCounts, faceConnects, outData, &stat);
}