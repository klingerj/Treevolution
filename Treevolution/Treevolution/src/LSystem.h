#ifndef LSystem_H_
#define LSystem_H_

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

class LSystem
{
public:
    typedef std::pair<glm::vec3, glm::vec3> Branch;

public:
    LSystem();
    ~LSystem() {}

    // Set/get inputs
    void loadProgram(const std::string& fileName);
    void loadProgramFromString(const std::string& program);
    void setDefaultAngle(float degrees);
    void setDefaultStep(float distance);

    float getDefaultAngle() const;
    float getDefaultStep() const;
    const std::string& getGrammarString() const;

    // Iterate grammar
    const std::string& getIteration(unsigned int n);

    // Get geometry from running the turtle
    void process(unsigned int n, 
        std::vector<Branch>& branches);

protected:
    void reset();
    void addProduction(std::string line);
    std::string iterate(const std::string& input);
    
    std::map<std::string, std::string> productions;
    std::vector<std::string> iterations;
    std::vector<Branch> bboxes;
    std::string current;
    float mDfltAngle;
    float mDfltStep;
    std::string mGrammar;

    class Turtle
    {
    public:
        Turtle();
        Turtle(const Turtle& t);
        Turtle& operator=(const Turtle& t);

        void moveForward(float distance);
        void applyUpRot(float degrees);
        void applyLeftRot(float degrees);
        void applyForwardRot(float degrees);

        glm::vec3 pos;
        glm::vec4 up;
        glm::vec4 forward;
        glm::vec4 left;
    };
};

#endif