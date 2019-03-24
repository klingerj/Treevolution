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

public:
    VolumetricFitnessEval(float gridCellSize);
    ~VolumetricFitnessEval();

    // Returns the evaluated fitness score.
    int Evaluate() const override;

    // Set the reference volumetric grid
    void SetGrids(const Mesh& mesh);

    // Set the current volumetric grid
    //void SetCurrentGrid(const std::vector<Triangle>& mesh);
};
