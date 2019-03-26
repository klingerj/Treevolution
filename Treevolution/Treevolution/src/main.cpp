#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

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
const float camMoveSensitivity = 0.01f;

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
    //DrawableLine lines = DrawableLine();
    //Line l;
    //l.start = glm::vec3(0.0f, 0.0f, 0.0f);
    //l.end = glm::vec3(0.0f, 0.5f, 0.0f);
    //lines.addLineSegment(l);
    
    // Load base branch model
    Mesh branchMesh = Mesh();
    branchMesh.LoadFromFile("res/models/cube.obj");

	  // Create L-system
	  LSystem sys;
	  sys.setDefaultStep(0.1f);
	  sys.setDefaultAngle(30.0f);
	  sys.loadProgramFromString("F\nF->F[+F][-F]"); //taken from simple1.txt
    std::string iteratedStr = sys.getIteration(1);

    TreeStructure theTree = TreeStructure(1, iteratedStr, 0.0f, 90.0f, 1.0f, 3.0f);
    TreeStructure theTree2 = TreeStructure(2, iteratedStr, 0.0f, 90.0f, 1.0f, 3.0f);
    theTree.Crossover(&theTree2);
    Mesh treeMesh = theTree.GetTreeMesh(branchMesh);
    Mesh treeMesh2 = theTree2.GetTreeMesh(branchMesh);
    treeMesh.Create();
    treeMesh2.Create();

    // Load reference model
    /*Mesh referenceMesh = Mesh();
    referenceMesh.LoadFromFile("res/models/cubeOrigin5.obj");

    // Volumetric fitness evaluation
    FitnessEvalMethod* eval = new VolumetricFitnessEval(0.1f);
    VolumetricFitnessEval* volumetricEval = dynamic_cast<VolumetricFitnessEval*>(eval);
    volumetricEval->SetGrid(referenceMesh, 0);
    volumetricEval->SetGrid(treeMesh, 1);
    // TODO: better way to do this other than dynamic casting?

    DrawablePoints gridPoints;
    std::vector<glm::vec3> points = volumetricEval->GetGridPoints(1);
    for (auto p : points) {
        gridPoints.addPoint(p);
    }
    gridPoints.Create();

    glPointSize(4);*/

    while (!glfwWindowShouldClose(window)) {
        // Input handling
        processInput(window);
        
        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the scene
        flatShader.setCameraViewProj("cameraViewProj", camera.GetViewProj());
        flatShader.Draw(treeMesh);
        //flatShader.Draw(gridPoints);

        // Check/call events, swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    //lines.destroy();
    //referenceMesh.destroy();
    treeMesh.destroy();
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
