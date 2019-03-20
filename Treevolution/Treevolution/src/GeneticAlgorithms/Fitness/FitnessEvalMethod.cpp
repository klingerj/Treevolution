#include "FitnessEvalMethod.h"

FitnessEvalMethod::FitnessEvalMethod() {}
FitnessEvalMethod::~FitnessEvalMethod() {}

VolumetricFitnessEval::VolumetricFitnessEval(const glm::vec3& gridDim) {
    gridReference = new uint8_t[(int)(gridDim.x * gridDim.y * gridDim.z)];
    gridCurrent = new uint8_t[(int)(gridDim.x * gridDim.y * gridDim.z)];
}

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

void VolumetricFitnessEval::SetReferenceGrid(const std::vector<Triangle>& mesh) {
    
}
