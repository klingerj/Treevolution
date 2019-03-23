#include "TreeStructure.h"

#include <glm/gtc/matrix_transform.hpp>

#define Deg2Rad 0.017453292519943295769236907684886

TreeNode::TreeNode() : TreeNode(0, -1.0f)
{

}

TreeNode::TreeNode(char c, float f) : name(c), param(f)
{
    axis = glm::vec3(0.f);
}

TreeNode::TreeNode(char c, float f, glm::vec3 &a) : name(c), param(f), axis(a)
{

}

TreeStructure::TreeStructure(std::string gram, float minAngle, float maxAngle, 
    float minLen, float maxLen) : mGrammar(gram), mMinAngle(minAngle), mMaxAngle(maxAngle),
    mMinLen(minLen), mMaxLen(maxLen), mRoot(nullptr)
{
    mRoot = new TreeNode[25];
    for (int i = 0; i < 25; ++i)
    {
        mRoot[i] = TreeNode();
    }

    ConstructTree(mRoot, gram);

    currTurtle  = Turtle();

    std::srand(2019);
}

void TreeStructure::ConstructTree(TreeNode *root, std::string substring)
{
    while (substring.length() != 0)
    {
        char sym = substring.at(0);

        if (sym == 'F' || sym == '-')
        {
            TreeNode* next = AddChildToNode(root, sym);
            ConstructTree(next, substring.substr(1, substring.length() - 1));
        }
        if (sym == '[')
        {
            nodeStack.push(root);
            ConstructTree(root, substring.substr(1, substring.length() - 1));
        }
        if (sym == ']')
        {
            TreeNode* returnNode = nodeStack.top();
            nodeStack.pop();
            ConstructTree(returnNode, substring.substr(1, substring.length() - 1));
        }
    }
}

TreeNode* TreeStructure::GetNextFreeNode()
{
    for (int i = 0; i < 25; ++i)
    {
        if (mRoot[i].GetName() == 0) {
            return mRoot + i;
        }
    }

    // TODO allocate a new linked list
}

TreeNode* TreeStructure::AddChildToNode(TreeNode* parent, char c) 
{
    TreeNode* freeNode = GetNextFreeNode();

    float p = 0.f;
    if (c == 'F')
    {
        // generate random float between mMinLen and mMaxLen 
        p = (float(std::rand()) / RAND_MAX) * (mMaxLen - mMinLen) + mMinLen;
        (*freeNode) = TreeNode(c, p);
    }
    else if (c == '-')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float y = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float z = (float(std::rand()) / RAND_MAX) * 2 - 1;
        glm::vec3 a(x, y, z);

        p = (float(std::rand()) / RAND_MAX) * (mMaxAngle - mMinAngle) + mMinAngle;
        (*freeNode) = TreeNode(c, p, a);
    }

    parent->children.push_back(freeNode);

    return freeNode;
}

void TreeStructure::processNode(TreeNode* currNode, Mesh &baseMesh)
{
    // perform the desired action for this node
    if (currNode->name == 'F')
    {
        // store current position then find the next position
        glm::vec3 startPos = currTurtle.pos;
        currTurtle.moveForward(currNode->param);
        glm::vec3 endPos = currTurtle.pos;

        // find the scale and translation of the branch geometry
        float halfDist = glm::length(endPos - startPos) / 2.f;
        glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(1.f, halfDist, 1.f));
        glm::mat4 trans = glm::translate(glm::mat4(1.0), ((endPos - startPos) / 2.f));

        // find the rotation of the branch geometry
        glm::mat4 rot = glm::mat4(currTurtle.left, currTurtle.up, 
            currTurtle.forward, glm::vec4(0.0));

        // create a transormation matrix
        glm::mat4 transform = trans * rot * scale;

        // add all the triangles of baseMesh modified by this matrix to tris
        std::vector<Triangle> origTris = baseMesh.GetTriangles();
        for (Triangle t : origTris)
        {
            std::vector<glm::vec3> oldPts = t.GetPoints();

            Triangle newT;
            newT.AppendVertex(transform * glm::vec4(oldPts[0], 1.0));
            newT.AppendVertex(transform * glm::vec4(oldPts[1], 1.0));
            newT.AppendVertex(transform * glm::vec4(oldPts[2], 1.0));

            tris.push_back(newT);
        }
    }
    else if (currNode->name == '-')
    {
        currTurtle.applyRot(currNode->axis, currNode->param);
    }

    for (TreeNode* child : currNode->children)
    {
        // begin new branch
        turtStack.push(currTurtle);

        // process child's subtree
        processNode(child, baseMesh);

        // end branch
        currTurtle = turtStack.top();
        turtStack.pop();
    }
}

Mesh TreeStructure::GetTreeMesh(Mesh &baseMesh)
{
    tris.clear();
    
    // clear turtle stack
    while (!turtStack.empty()) { turtStack.pop(); }

    currTurtle = Turtle();
    
    Mesh output;

    //processNode(mRoot, cubeboi);

    // set tris to traingles of this mesh
    output.SetTriangles(tris);

    return output;
}

TreeStructure::Turtle::Turtle() :
    pos(0, 0, 0),
    up(0, 1, 0, 0),
    forward(0, 0, 1, 0),
    left(1, 0, 0, 0)
{
}

TreeStructure::Turtle::Turtle(const TreeStructure::Turtle& t)
{
    pos = t.pos;
    up = t.up;
    forward = t.forward;
    left = t.left;
}

TreeStructure::Turtle& TreeStructure::Turtle::operator=(const TreeStructure::Turtle& t)
{
    if (&t == this) return *this;

    pos = t.pos;
    up = t.up;
    forward = t.forward;
    left = t.left;
    return *this;
}

void TreeStructure::Turtle::moveForward(float length)
{
    pos = pos + length * glm::vec3(forward);
}

void TreeStructure::Turtle::applyRot(glm::vec3 axis, float angle)
{
    glm::mat4 mat = glm::rotate(glm::mat4(1.0), float(Deg2Rad*angle), axis); // Y axis
    glm::mat4 world2local = glm::mat4(left, up, forward, glm::vec4(0.0));
    up = world2local * mat * glm::vec4(0, 1, 0, 0);
    left = world2local * mat * glm::vec4(1, 0, 0, 0);
    forward = world2local * mat * glm::vec4(0, 0, 1, 0);
}