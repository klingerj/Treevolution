#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "OpenGL/ShaderProgram.h"
#include "Scene/DrawableLine.h"
#include "Scene/DrawablePoints.h"
#include "LSystem.h"
#include "GeneticAlgorithms/Fitness/FitnessEvalMethod.h"
#include "GeneticAlgorithms/TreeStructure.h"
#include "Scene/Mesh.h"
#include "Scene/Camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Camera camera = Camera(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), 0.7853981634f, 1.3333f, 0.01f, 100.0f);
const float camMoveSensitivity = 0.02f;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.TranslateAlongLook(camMoveSensitivity);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.TranslateAlongLook(-camMoveSensitivity);
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.TranslateAlongRight(-camMoveSensitivity * 0.2f);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.TranslateAlongRight(camMoveSensitivity * 0.2f);
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.RotateAboutRight(-camMoveSensitivity * 0.2f);
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.RotateAboutRight(camMoveSensitivity * 0.2f);
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera.RotateAboutUp(camMoveSensitivity * 0.2f);
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera.RotateAboutUp(-camMoveSensitivity * 0.2f);
    }
    else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.TranslateAlongUp(-camMoveSensitivity);
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.TranslateAlongUp(camMoveSensitivity);
    }
}

int main() {
    // Setup GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Treevolution", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Setup GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Load shader programs
    ShaderProgram flatShader = ShaderProgram("src/Shaders/flat.vert", "src/Shaders/flat.frag");

    // Array/Buffer Objects
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Scene data
    
    // Load base branch model
    Mesh branchMesh = Mesh();
    branchMesh.LoadFromFile("res/models/cube.obj");

	  // Create L-system
	  LSystem sys;
	  sys.setDefaultStep(0.1f);
	  sys.setDefaultAngle(30.0f);
	  sys.loadProgramFromString("F\nF->F[+F][-F]"); //taken from simple1.txt
    std::string iteratedStr = sys.getIteration(2);

    /*TreeStructure theTree = TreeStructure(1, iteratedStr, 0.0f, 90.0f, 1.0f, 3.0f);
    TreeStructure theTree2 = TreeStructure(2, iteratedStr, 0.0f, 90.0f, 1.0f, 3.0f);
    theTree.Crossover(&theTree2);
    Mesh treeMesh = theTree.GetTreeMesh(branchMesh);
    Mesh treeMesh2 = theTree2.GetTreeMesh(branchMesh);
    treeMesh.Create();
    treeMesh2.Create();*/

    // Load reference model
    Mesh referenceMesh = Mesh();
    referenceMesh.LoadFromFile("res/models/complexBoi.obj");

    // Volumetric fitness evaluation
    FitnessEvalMethod* eval = new VolumetricFitnessEval(0.22f);
    VolumetricFitnessEval* volumetricEval = dynamic_cast<VolumetricFitnessEval*>(eval);
    volumetricEval->SetGrid(referenceMesh, 0);


    //volumetricEval->SetGrid(treeMesh, 1);
    // TODO: better way to do this other than dynamic casting?

    /*DrawablePoints gridPoints;
    std::vector<glm::vec3> points = volumetricEval->GetGridPoints(1);
    for (auto p : points) {
        gridPoints.addPoint(p);
    }
    gridPoints.Create();*/

    

    const int elitism = 40; // must be even!!!!!!
    std::vector<TreeStructure> population;
    constexpr int popSize = 200;
    population.reserve(popSize * 2);
    for (int i = 0; i < popSize; ++i) {
        population.emplace_back(std::move(TreeStructure(i, iteratedStr, 0.0f, 90.0f, 1.0f, 3.0f)));
    }

    std::vector<TreeStructure> newPopulation;
    newPopulation.reserve(popSize * 2);

    constexpr int numGenerations = 50;
    for (int i = 0; i < numGenerations; ++i) {
        std::cout << "New Generation: " << i << std::endl;
        // Compute fitness scores
        for (int j = 0; j < popSize; ++j) {
            //std::cout << "Handling pop member: " << j << std::endl;
            Mesh treeMesh = population[j].GetTreeMesh(branchMesh); // Get the current pop member's mesh
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

    std::vector<Mesh> treeMeshes;
    for (int i = 0; i < elitism; ++i) {
        std::cout << "Fitness: " << population[i].fitnessScore << std::endl;
        treeMeshes.push_back(population[i].GetTreeMesh(branchMesh));
        treeMeshes[i].Create();
    }

    DrawablePoints gridPoints;
    std::vector<glm::vec3> points = volumetricEval->GetGridPoints(0);
    for (auto p : points) {
        gridPoints.addPoint(p);
    }
    std::cout << "num points: " << points.size() << std::endl;
    gridPoints.Create();
    glPointSize(4);

    while (!glfwWindowShouldClose(window)) {
        // Input handling
        processInput(window);
        
        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the scene

        // Model matrices
        flatShader.setCameraViewProj("cameraViewProj", camera.GetViewProj());
        glm::mat4 model = glm::mat4(1.0f);
        
        for (int i = 0; i < elitism; ++i) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * i, 0.0, 0.0f));
            flatShader.SetModelMatrix("model", model);
            flatShader.Draw(treeMeshes[i]);
            flatShader.Draw(gridPoints);
        }

        // Check/call events, swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    //lines.destroy();
    //referenceMesh.destroy();
    for (int i = 0; i < elitism; ++i) {
        treeMeshes[i].destroy();
    }
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
