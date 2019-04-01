#include "FitnessEvalMethod.h"

FitnessEvalMethod::FitnessEvalMethod() {}
FitnessEvalMethod::~FitnessEvalMethod() {}

VolumetricFitnessEval::VolumetricFitnessEval(float gridCellSize) : gridCellSize(gridCellSize), gridReference(nullptr), gridCurrent(nullptr) {}

VolumetricFitnessEval::~VolumetricFitnessEval() {
    if (gridReference) {
        delete gridReference;
    }
    if (gridCurrent) {
        delete gridCurrent;
    }
}
#include <iostream>
int VolumetricFitnessEval::Evaluate() const {
    int score = 0;
    for (int i = 0; i < (int)std::ceil(gridDim.x * gridDim.y * gridDim.z); ++i) {
        score += 200 * (int)(gridReference[i] & gridCurrent[i] && gridReference[i]);
        score -= (int)(gridReference[i] ^ gridCurrent[i]);
    }
    //std::cout << "Score: " << score << std::endl;
    return score;
}

void VolumetricFitnessEval::SetGrid(const Mesh& mesh, uint8_t gridType) {
    uint8_t* grid;
    if (gridType == 0)
    {
        gridMin = mesh.GetMinPos();
        gridDim = glm::ceil((mesh.GetMaxPos() - gridMin) / glm::vec3(gridCellSize));
        gridReference = new uint8_t[(int)std::ceil((gridDim.x * gridDim.y * gridDim.z))];
        grid = gridReference;
    }
    else
    {
        if (gridCurrent == nullptr) {
            gridCurrent = new uint8_t[(int)std::ceil((gridDim.x * gridDim.y * gridDim.z))];
        }
        grid = gridCurrent;
    }

    for (int i = 0; i < gridDim.x; ++i)
    {
        for (int j = 0; j < gridDim.y; ++j)
        {
            for (int k = 0; k < gridDim.z; ++k)
            {
                const int idx = k + (int)(j * gridDim.z) + (int)(i * gridDim.z * gridDim.y);
                grid[idx] = 0;
                const glm::vec3 center = gridMin +                           // start at grid min
                    glm::vec3(gridCellSize * glm::vec3(i, j, k));// + // grid corners to the index in question
                                         //gridCellSize * 0.5f;                // move to grid center
                if (mesh.Contains(center))
                {
                    grid[idx] = 1;
                }
            }
        }
    }
}

std::vector<glm::vec3> VolumetricFitnessEval::GetGridPoints(uint8_t gridType) {
    std::vector<glm::vec3> points;
    points.reserve((int)std::ceil(gridDim.x * gridDim.y * gridDim.z));

    uint8_t* grid;
    if (gridType == 0)
    {
        grid = gridReference;
    }
    else
    {
        grid = gridCurrent;
    }

    for (int i = 0; i < gridDim.x; ++i)
    {
        for (int j = 0; j < gridDim.y; ++j)
        {
            for (int k = 0; k < gridDim.z; ++k)
            {
                const int idx = k + (int)(j * gridDim.z) + (int)(i * gridDim.z * gridDim.y);
                const glm::vec3 center = gridMin +                           // start at grid min
                    gridCellSize * glm::vec3(i, j, k);// + // grid corners to the index in question
                    //gridCellSize * 0.5f;                // move to grid center
                if (grid[idx])
                {
                    points.push_back(center);
                }
            }
        }
    }
    return points;
}
