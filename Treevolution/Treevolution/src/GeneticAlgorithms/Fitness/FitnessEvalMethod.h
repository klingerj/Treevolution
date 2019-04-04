#pragma once

#include "glm/glm.hpp"

#include <vector>

#include "../../Scene/Mesh.h"

class FitnessEvalMethod {
public:
    FitnessEvalMethod();
    ~FitnessEvalMethod();

    /* Returns the evaluated fitness score.
     * Must be implemented by the subclasses.
     */
    virtual int Evaluate() const = 0;
};

class VolumetricFitnessEval : public FitnessEvalMethod {
private:
    // The grid containing volumetric info of the reference model
    uint8_t* gridReference;
    // The grid containing volumetric info of the current procedural model
    uint8_t* gridCurrent;

    float gridCellSize;
    glm::vec3 gridDim;
    glm::vec3 gridDimCurrent;
    glm::vec3 gridMin;
    glm::vec3 gridMinCurrent;

public:
    VolumetricFitnessEval(float gridCellSize);
    ~VolumetricFitnessEval();

    // Returns the evaluated fitness score.
    int Evaluate() const override;

    // Set the reference volumetric grid
    void SetGrid(const Mesh& mesh, uint8_t gridType);
    std::vector<glm::vec3> GetGridPoints(uint8_t gridType);
};
