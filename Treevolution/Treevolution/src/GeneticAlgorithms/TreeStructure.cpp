#include "TreeStructure.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define Deg2Rad 0.017453292519943295769236907684886

TreeNode::TreeNode() : TreeNode(0, -1.0f)
{

}

TreeNode::TreeNode(TreeNode* t) : name(t->name), param(t->param),
    axis(t->axis), children(t->children)
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
    mRoot = new TreeNode();
    nodeList.push_back(mRoot);

    ConstructTree(mRoot, gram);

    currTurtle  = Turtle();

    std::srand(2019);
}

TreeStructure::TreeStructure(TreeStructure* t, TreeNode* root) : mGrammar(t->GetGram()), mMinAngle(t->GetMinAngle()), 
    mMaxAngle(t->GetMaxAngle()), mMinLen(t->GetMinLen()), mMaxLen(t->GetMaxLen()), mRoot(root)
{
    nodeList.clear();
    CreateNodeList(mRoot);
}

void TreeStructure::ConstructTree(TreeNode *root, std::string substring)
{
    if (substring.length() != 0)
    {
        char sym = substring.at(0);

        if (sym == 'F' || sym == '-' || sym == '+')
        {
            TreeNode* next = AddChildToNode(root, sym);
            ConstructTree(next, substring.substr(1, substring.length() - 1));
        }
        else if (sym == '[')
        {
            nodeStack.push(root);
            ConstructTree(root, substring.substr(1, substring.length() - 1));
        }
        else if (sym == ']')
        {
            TreeNode* returnNode = nodeStack.top();
            nodeStack.pop();
            ConstructTree(returnNode, substring.substr(1, substring.length() - 1));
        }
    }
}

TreeNode* TreeStructure::AddChildToNode(TreeNode* parent, char c) 
{
    TreeNode* newNode = new TreeNode();

    float p = 0.f;
    if (c == 'F')
    {
        // generate random float between mMinLen and mMaxLen 
        p = (float(std::rand()) / RAND_MAX) * (mMaxLen - mMinLen) + mMinLen;
        //p = 1.0f;
        (*newNode) = TreeNode(c, p);
    }
    else if (c == '-')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float y = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float z = (float(std::rand()) / RAND_MAX) * 2 - 1;
        glm::vec3 a(x, y, z);
        a = glm::normalize(a);

        //a = glm::vec3(0, 0, 1);

        p = -1.f * ((float(std::rand()) / RAND_MAX) * (mMaxAngle - mMinAngle) + mMinAngle);
        //p = -45.0f;
        (*newNode) = TreeNode(c, p, a);
    }
    else if (c == '+')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float y = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float z = (float(std::rand()) / RAND_MAX) * 2 - 1;
        glm::vec3 a(x, y, z);
        a = glm::normalize(a);

        //a = glm::vec3(0, 0, 1);

        p = (float(std::rand()) / RAND_MAX) * (mMaxAngle - mMinAngle) + mMinAngle;
        //p = 45.0f;
        (*newNode) = TreeNode(c, p, a);
    }

    parent->children.push_back(newNode);
    nodeList.push_back(newNode);

    return newNode;
}

void TreeStructure::CreateNodeList(TreeNode* root)
{
    nodeList.push_back(root);
    for (TreeNode* c : root->children)
    {
        CreateNodeList(c);
    }
}

void ReplaceSubtree(TreeNode* root, TreeNode* swapNode, TreeNode* toSwap)
{
    if (root == nullptr)
    {
        // FAIL
        std::cout << "Subtree is not in original tree :(";
        return;
    }
    for (int i = 0; i < root->children.size(); i++)
    {
        if (root->children[i] == swapNode)
        {
            root->children[i] = toSwap;
            return;
        }
    }
    // The swapNode is not a child of this root, so we iterate on the children
    for (TreeNode* c : root->children)
    {
        ReplaceSubtree(c, swapNode, toSwap);
    }
}

std::vector<TreeStructure> TreeStructure::Crossover(TreeStructure* parent2)
{
    // get a random node from both this and parent2
    int rand1 = std::rand() % this->nodeList.size();
    int rand2 = std::rand() % parent2->GetCount();
    TreeNode* subtree1 = this->GetNodeAtCount(rand1);
    TreeNode* subtree2 = parent2->GetNodeAtCount(rand2);

    // create child root nodes which are copies of parents
    TreeNode* child1 = new TreeNode(this->mRoot);
    TreeNode* child2 = new TreeNode(parent2->GetRoot());
    // swap the subtrees
    ReplaceSubtree(child1, subtree1, subtree2);
    ReplaceSubtree(child2, subtree2, subtree1);

    // make the children into TreeStructures and return
    std::vector<TreeStructure> children;
    children.push_back(TreeStructure(this, child1));
    children.push_back(TreeStructure(parent2, child2));

    return children;
}

void TreeStructure::Grow(TreeNode* toGrow)
{

}

void TreeStructure::Cut(TreeNode* toCut)
{

}

void TreeStructure::Alter()
{
    // get a random node
    int rand = std::rand() % this->nodeList.size();
    TreeNode* gene = this->GetNodeAtCount(rand);

    // recompute random parameters
    if (gene->name == 'F')
    {
        // generate random float between mMinLen and mMaxLen 
        gene->param = (float(std::rand()) / RAND_MAX) * (mMaxLen - mMinLen) + mMinLen;
    }
    else if (gene->name == '+')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float y = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float z = (float(std::rand()) / RAND_MAX) * 2 - 1;
        glm::vec3 a(x, y, z);
        gene->axis = glm::normalize(a);

        gene->param = (float(std::rand()) / RAND_MAX) * (mMaxAngle - mMinAngle) + mMinAngle;
    }
    else if (gene->name == '-')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float y = (float(std::rand()) / RAND_MAX) * 2 - 1;
        float z = (float(std::rand()) / RAND_MAX) * 2 - 1;
        glm::vec3 a(x, y, z);
        gene->axis = glm::normalize(a);

        gene->param = -1.f * ((float(std::rand()) / RAND_MAX) * (mMaxAngle - mMinAngle) + mMinAngle);
    }
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
        const float dist = glm::length(startPos - endPos);
        const float radius = 0.3f;
        const glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(radius, dist, radius));
        
        const glm::vec3 newStart = glm::vec3(startPos.x, startPos.y, startPos.z);
        const glm::vec3 newEnd = glm::vec3(endPos.x, endPos.y, endPos.z);
        const glm::mat4 trans = glm::translate(glm::mat4(1.0), (newEnd - newStart) * 0.5f + newStart);

        // find the rotation of the branch geometry
        const glm::mat4 rot = glm::mat4(currTurtle.left, currTurtle.up,
            currTurtle.forward, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        // create a transormation matrix
        const glm::mat4 transform = trans * rot * scale;

        // add all the triangles of baseMesh modified by this matrix to tris
        std::vector<Triangle> origTris = baseMesh.GetTriangles();
        for (Triangle t : origTris)
        {
            std::vector<glm::vec3> oldPts = t.GetPoints();

            Triangle newT;
            newT.AppendVertex(transform * glm::vec4(oldPts[0], 1.0));
            newT.AppendVertex(transform * glm::vec4(oldPts[1], 1.0));
            newT.AppendVertex(transform * glm::vec4(oldPts[2], 1.0));
            newT.ComputePlaneNormal();

            tris.push_back(newT);
        }
    }
    else if (currNode->name == '-' || currNode->name == '+')
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

    processNode(mRoot, baseMesh);

    // set tris to traingles of this mesh
    output.SetTriangles(tris);
    output.SetMinMaxPosFromTris();

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
    pos = pos + length * glm::vec3(up);
}

void TreeStructure::Turtle::applyRot(glm::vec3 axis, float angle)
{
    glm::mat4 mat = glm::rotate(glm::mat4(1.0), float(Deg2Rad*angle), axis);
    glm::mat4 world2local = glm::mat4(left, up, forward, glm::vec4(0.0));
    up = world2local * mat * glm::vec4(0, 1, 0, 0);
    left = world2local * mat * glm::vec4(1, 0, 0, 0);
    forward = world2local * mat * glm::vec4(0, 0, 1, 0);
}
