#include "TreeStructure.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define Deg2Rad 0.017453292519943295769236907684886

TreeNode::TreeNode() : TreeNode(0, -1.0f, nullptr)
{

}

TreeNode::TreeNode(TreeNode* t) : name(t->name), param(t->param), param2(t->param2),
    axis(t->axis)
{

}

TreeNode::TreeNode(char c, float f, TreeNode* parent) : name(c), param(f),
    parent(parent)
{
    axis = glm::vec3(0.f);
}

TreeNode::TreeNode(char c, float f, glm::vec3 &a, TreeNode* parent) : 
    name(c), param(f), axis(a), parent(parent)
{

}

TreeNode::TreeNode(char c, float f, float f2, glm::vec3 &a, TreeNode* parent) :
    name(c), param(f), param2(f2), axis(a), parent(parent)
{

}

TreeNode::~TreeNode()
{
    for (TreeNode* c : this->children)
    {
        delete c;
    }
}

TreeStructure::TreeStructure(int id, std::string gram, float minAngle, float maxAngle, 
    float minLen, float maxLen) : mId(id), mGrammar(gram), mMinAngle(minAngle), mMaxAngle(maxAngle),
    mMinLen(minLen), mMaxLen(maxLen), mRoot(nullptr), fitnessScore(0)
{
    mGenerator.seed(mId);
    mAxisDist = std::uniform_real_distribution<float>(-1.0, 1.0);
    mAngleDist = std::uniform_real_distribution<float>(mMinAngle, mMaxAngle);
    mLenDist = std::uniform_real_distribution<float>(mMinLen, mMaxLen);
    
    mRoot = new TreeNode();
    nodeList.push_back(mRoot);

    ConstructTree(mRoot, gram);

    currTurtle  = Turtle();
}

TreeStructure::TreeStructure(TreeStructure* t) : mId(t->GetId()), mGrammar(t->GetGram()), 
    mMinAngle(t->GetMinAngle()), mMaxAngle(t->GetMaxAngle()), mMinLen(t->GetMinLen()), mMaxLen(t->GetMaxLen())
{
    mGenerator.seed(mId);
    mAxisDist = std::uniform_real_distribution<float>(-1.0, 1.0);
    mAngleDist = std::uniform_real_distribution<float>(mMinAngle, mMaxAngle);
    mLenDist = std::uniform_real_distribution<float>(mMinLen, mMaxLen);
    
    nodeList.clear();
    TreeNode* root = t->GetRoot();
    mRoot = new TreeNode(root);
    CreateCopyNodeList(mRoot, root);
}

TreeStructure::~TreeStructure()
{
    //if (mRoot) delete mRoot;
}

void TreeStructure::ConstructTree(TreeNode *root, std::string substring)
{
    if (substring.length() != 0)
    {
        char sym = substring.at(0);

        if (sym == 'F' || sym == '-' || sym == '+' || sym == 'X')
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

    if (c == 'F')
    {
        // generate random float between mMinLen and mMaxLen 
        float p = mLenDist(mGenerator);
        (*newNode) = TreeNode(c, p, parent);
    }
    else if (c == '-')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = mAxisDist(mGenerator);
        float y = mAxisDist(mGenerator);
        float z = mAxisDist(mGenerator);
        glm::vec3 a(x, y, z);
        a = glm::normalize(a);

        float p = -1.f * mAngleDist(mGenerator);
        (*newNode) = TreeNode(c, p, a, parent);
    }
    else if (c == '+')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = mAxisDist(mGenerator);
        float y = mAxisDist(mGenerator);
        float z = mAxisDist(mGenerator);
        glm::vec3 a(x, y, z);
        a = glm::normalize(a);

        float p = mAngleDist(mGenerator);
        (*newNode) = TreeNode(c, p, a, parent);
    }
    else if (c == 'X')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = mAxisDist(mGenerator);
        float y = mAxisDist(mGenerator);
        float z = mAxisDist(mGenerator);
        glm::vec3 a(x, y, z);
        a = glm::normalize(a);

        float p = mAngleDist(mGenerator);
        float p2 = mLenDist(mGenerator);
        (*newNode) = TreeNode(c, p, p2, a, parent);
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

void TreeStructure::CreateCopyNodeList(TreeNode* root, TreeNode* toCopy)
{
    nodeList.push_back(root);
    for (TreeNode* c : toCopy->children)
    {
        TreeNode* newC = new TreeNode(c);
        root->children.push_back(newC);
        newC->parent = root;
        CreateCopyNodeList(newC, c);
    }
}

int GetIndexInParentList(TreeNode* node)
{
    TreeNode* par = node->parent;
    int idx = -1;
    if (par == nullptr)
    {
        return idx;
    }
    for (int i = 0; i < par->children.size(); i++)
    {
        if (par->children[i] == node) { idx = i; }
    }
    return idx;
}

void TreeStructure::Crossover(TreeStructure* parent2)
{
    // get a random node from both this and parent2
    std::uniform_real_distribution<float> chooseDist(0.0f, (float)(this->nodeList.size()));
    int rand1 = (int)floor(chooseDist(mGenerator));
    std::uniform_real_distribution<float> chooseDist2(0.0f, (float)(parent2->GetCount()));
    int rand2 = (int)floor(chooseDist2(mGenerator));
    TreeNode* subtree1 = this->GetNodeAtCount(rand1);
    TreeNode* subtree2 = parent2->GetNodeAtCount(rand2);
    int idx1 = GetIndexInParentList(subtree1);
    int idx2 = GetIndexInParentList(subtree2);

    // swap the subtrees
    if (idx1 == -1 && idx2 == -1)
    {
        //this->mRoot = subtree2;
        //parent2->SetRoot(subtree1);
    }
    else if (idx1 == -1)
    {
        //this->mRoot = subtree2;
        //subtree2->parent->children[idx2] = subtree1;
    }
    else if (idx2 == -1)
    {
        //subtree1->parent->children[idx1] = subtree2;
        //parent2->SetRoot(subtree1);
    }
    else
    {
        subtree1->parent->children[idx1] = subtree2;
        subtree2->parent->children[idx2] = subtree1;
        TreeNode* temp = subtree1->parent;
        subtree1->parent = subtree2->parent;
        subtree2->parent = temp;
    }

    nodeList.clear();
    CreateNodeList(mRoot);

    parent2->ClearNodeList();
    parent2->CreateNodeList(parent2->mRoot);
}
void TreeStructure::Grow(const std::map<std::string, std::vector<std::string>> &rules)
{
    if (nodeList.size() <= 2) {
        return;
    }
    // get a random node
    std::uniform_real_distribution<float> chooseDist(0.0f, (float)(this->nodeList.size()));
    int rand = (int)floor(chooseDist(mGenerator));
    TreeNode* gene = this->GetNodeAtCount(rand);

    // if char of gene has a rule apply it, otherwise add an F
    std::string s(1, gene->name);
    if (rules.count(s) > 0)
    {
        std::uniform_real_distribution<float> gramDist(0.0f, (float)(rules.at(s).size()));
        int rand2 = (int)floor(gramDist(mGenerator));
        std::string add = rules.at(s)[rand2];
        ConstructTree(gene, add);
    }
    else
    {
        AddChildToNode(gene, 'F');
    }

    // repopulate the node list after the change
    nodeList.clear();
    CreateNodeList(mRoot);
}

void TreeStructure::Cut()
{
    if (nodeList.size() <= 2) return;
    // get a random node
    std::uniform_real_distribution<float> chooseDist(2.0, (float)(this->nodeList.size() - 1));
    int rand = (int)floor(chooseDist(mGenerator));
    TreeNode* gene = this->GetNodeAtCount(rand);

    if (gene != mRoot && gene->parent && gene->parent->children.size() > 0)
    {
        int idx = GetIndexInParentList(gene);
        gene->parent->children.erase(gene->parent->children.begin() + idx);

        delete gene;
    }

    // repopulate the node list after the change
    nodeList.clear();
    CreateNodeList(mRoot);
}

void TreeStructure::Alter()
{
    if (nodeList.size() <= 2) return;
    // get a random node
    std::uniform_real_distribution<float> chooseDist(2.0, (float)(this->nodeList.size() - 1));
    int rand = (int)floor(chooseDist(mGenerator));
    TreeNode* gene = this->GetNodeAtCount(rand);

    // recompute random parameters
    if (gene->name == 'F')
    {
        // generate random float between mMinLen and mMaxLen 
        gene->param = mLenDist(mGenerator); 
    }
    else if (gene->name == '+')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = mAxisDist(mGenerator);
        float y = mAxisDist(mGenerator);
        float z = mAxisDist(mGenerator);
        glm::vec3 a(x, y, z);
        gene->axis = glm::normalize(a);

        gene->param = mAngleDist(mGenerator); 
    }
    else if (gene->name == '-')
    {
        // generate three random numbers for axis and a random number for angle val
        float x = mAxisDist(mGenerator); 
        float y = mAxisDist(mGenerator); 
        float z = mAxisDist(mGenerator); 
        glm::vec3 a(x, y, z);
        gene->axis = glm::normalize(a);

        gene->param = mAngleDist(mGenerator);
    }
    else if (gene->name == 'X')
    {
        float x = mAxisDist(mGenerator);
        float y = mAxisDist(mGenerator);
        float z = mAxisDist(mGenerator);
        glm::vec3 a(x, y, z);
        gene->axis = glm::normalize(a);

        gene->param = mAngleDist(mGenerator);
        gene->param2 = mLenDist(mGenerator);
    }
}

int TreeStructure::Mutate(const std::map<std::string, std::vector<std::string>> &rules)
{
    std::uniform_real_distribution<float> chooseDist(0.0, 10.0);
    int action = (int)chooseDist(mGenerator);

    if (action < 1)
    {
        Grow(rules);
    }
    else if (action < 2)
    {
        Cut();
    }
    else if (action < 3)
    {
        Alter();
    }

    return action;
}

void TreeStructure::processNode(TreeNode* currNode, Mesh &baseMesh, Mesh &leafMesh)
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
    else if (currNode->name == 'X')
    {
        // translation to current position
        glm::vec3 pos = currTurtle.pos;
        const glm::mat4 trans = glm::translate(glm::mat4(1.0), pos);
        const glm::mat4 pivotTrans = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.25, 0.0));

        // find the rotation of the leaf geometry
        const glm::mat4 rot = glm::rotate(glm::mat4(1.0), currNode->param, currNode->axis);

        const glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.1*currNode->param2, 0.25*currNode->param2, 0.1*currNode->param2));

        // create a transormation matrix
        const glm::mat4 transform = trans*rot*pivotTrans*scale;

        // add all the triangles of baseMesh modified by this matrix to tris
        std::vector<Triangle> origTris = leafMesh.GetTriangles();
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
        processNode(child, baseMesh, leafMesh);

        // end branch
        currTurtle = turtStack.top();
        turtStack.pop();
    }
}

Mesh TreeStructure::GetTreeMesh(Mesh &baseMesh, Mesh &leafMesh)
{
    tris.clear();
    
    // clear turtle stack
    while (!turtStack.empty()) { turtStack.pop(); }

    currTurtle = Turtle();
    
    Mesh output;

    processNode(mRoot, baseMesh, leafMesh);

    // set tris to traingles of this mesh
    //if (tris.size() > 0) {
        //std::cout << "Num tris: " << tris.size() << std::endl;
        output.SetTriangles(tris);
        output.SetMinMaxPosFromTris();
    //}

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
