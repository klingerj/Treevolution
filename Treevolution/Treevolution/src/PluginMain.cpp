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

#include "TreevolutionNode.h"

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

    //MGlobal::executeCommand("deleteUI -m $menu;");

    return status;
}