#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "TreevolutionNode.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

MStatus initializePlugin(MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin(obj, "TreevolutionPlugin", "1.0", "Any");

    // Register Command ????
    /*status = plugin.registerCommand("LSystemCmd", LSystemCmd::creator);
    if (!status) {
        status.perror("registerCommand");
        return status;
    }*/
    status = plugin.registerNode("TreevolutionNode", TreevolutionNode::id, TreevolutionNode::creator, TreevolutionNode::initialize);
    if (!status) {
        status.perror("registerNode");
        return status;
    }

    // create GUI script
    MGlobal::executeCommand("global string $gMainWindow;");
    MGlobal::executeCommand("setParent $gMainWindow;");
    MGlobal::executeCommand("string $menu = `menu -label \"Treevolution\" -parent $gMainWindow`;");
    MGlobal::executeCommand("menuItem -label \"Treevolution Tool\" -c treevoScript;");

    // setup opengl context
    // Setup GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(400, 400, "Treevolution", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        //return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Setup GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        //return -1;
    }

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin(obj);

    /*status = plugin.deregisterCommand("LSystemCmd");
    if (!status) {
        status.perror("deregisterCommand");
        return status;
    }*/
    status = plugin.deregisterNode(TreevolutionNode::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }

    MGlobal::executeCommand("deleteUI -m $menu;");

    return status;
}