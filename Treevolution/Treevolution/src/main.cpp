#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "OpenGL/ShaderProgram.h"
#include "Scene/DrawableLine.h"
#include "LSystem.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
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
    DrawableLine lines = DrawableLine();
    Line l;
    l.start = glm::vec3(0.0f, 0.0f, 0.0f);
    l.end = glm::vec3(0.0f, 0.5f, 0.0f);
    //lines.addLineSegment(l);
    
	// Create L-system
	LSystem sys;
	sys.setDefaultStep(0.1);
	sys.setDefaultAngle(30.0);
	sys.loadProgramFromString("F\nF->F[+F]F[-F]F"); //taken from simple1.txt

	// Run turtle
	std::vector<LSystem::Branch> branches;
	sys.process(2, branches); 

	// Create lines from branches
	for (LSystem::Branch b : branches)
	{
		Line l;
		l.start = b.first;
		l.end = b.second;
		lines.addLineSegment(l);
	}

    lines.create();

    while (!glfwWindowShouldClose(window)) {
        // Input handling
        processInput(window);
        
        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the scene
        flatShader.Draw(lines);

        // Check/call events, swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    lines.destroy();
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
