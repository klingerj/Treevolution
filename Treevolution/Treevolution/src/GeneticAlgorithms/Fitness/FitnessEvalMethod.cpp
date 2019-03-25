#include "FitnessEvalMethod.h"

FitnessEvalMethod::FitnessEvalMethod() {}
FitnessEvalMethod::~FitnessEvalMethod() {}

VolumetricFitnessEval::VolumetricFitnessEval(float gridCellSize) : gridCellSize(gridCellSize) {}

VolumetricFitnessEval::~VolumetricFitnessEval() {
    if (gridReference) {
        delete gridReference;
    }
    if (gridCurrent) {
        delete gridCurrent;
    }
}

int VolumetricFitnessEval::Evaluate() const {
    int score = 0;
    for (int i = 0; i < sizeof(*gridReference); ++i) {
        score += (int)(gridReference[i] == gridCurrent[i]);
    }
    return score;
}

void VolumetricFitnessEval::SetGrids(const Mesh& mesh) {
    const glm::vec3 gridMin = mesh.GetMinPos();
    const glm::vec3 gridMax = mesh.GetMaxPos();
    const glm::vec3 gridDim = glm::ceil((gridMax - gridMin) / glm::vec3(gridCellSize));
    gridReference = new uint8_t[(int)(gridDim.x * gridDim.y * gridDim.z)];
    gridCurrent = new uint8_t[(int)(gridDim.x * gridDim.y * gridDim.z)];
    for (int i = 0; i < gridDim.x; ++i) {
        for (int j = 0; j < gridDim.y; ++j) {
            for (int k = 0; k < gridDim.z; ++k) {
                const int idx = k + (int)(j * gridDim.z) + (int)(i * gridDim.z * gridDim.y);
                gridReference[idx] = 0;
                gridCurrent[idx] = 0;
                const glm::vec3 center = gridMin +                           // start at grid min
                                         glm::vec3(gridDim * gridCellSize) + // grid corners to the index in question
                                         gridCellSize / 2.0f;                // move to grid center
                if (mesh.Contains(center)) {
                    gridReference[idx] = 1;
                }
            }
        }
    }
}
