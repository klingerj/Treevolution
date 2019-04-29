#include "FitnessEvalMethod.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

int VolumetricFitnessEval::Evaluate() const {
    int score = 0;

    for (int i = 0; i < gridDimCurrent.x; ++i)
    {
        for (int j = 0; j < gridDimCurrent.y; ++j)
        {
            for (int k = 0; k < gridDimCurrent.z; ++k)
            {
                const int idx = k + (int)(j * gridDimCurrent.z) + (int)(i * gridDimCurrent.z * gridDimCurrent.y);
                const glm::vec3 center = gridMinCurrent +
                    glm::vec3(gridCellSize * glm::vec3(i, j, k));
                const glm::vec3 gridMaxRef = gridMin + gridDim * gridCellSize;
                if (center.x < gridMin.x || center.y < gridMin.y || center.z < gridMin.z ||
                    center.x > gridMaxRef.x || center.y > gridMaxRef.y || center.z > gridMaxRef.z) {
                    score -= 2;
                } else {
                    glm::vec3 index = glm::round((center - gridMin) / gridCellSize);
                    int idxRef = (int)(index.z + index.y * gridDim.z + index.x * gridDim.z * gridDim.y);
                    if (gridCurrent[idx] == gridReference[idxRef] && gridCurrent[idx] == 1) {
                        score += 200;
                    }
                    else if ((gridReference[idxRef] ^ gridCurrent[idx])) {
                        score--;
                    }
                }

            }
        }
    }
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
        gridDimCurrent = gridDim;
        gridMinCurrent = gridMin;
    }
    else
    {
        gridMinCurrent = mesh.GetMinPos();
        gridDimCurrent = glm::ceil((mesh.GetMaxPos() - gridMinCurrent) / glm::vec3(gridCellSize));
        if (gridCurrent) {
            delete gridCurrent;
            gridCurrent = nullptr;
        }
        if (gridCurrent == nullptr) {
            gridCurrent = new uint8_t[(int)std::ceil((gridDimCurrent.x * gridDimCurrent.y * gridDimCurrent.z))];
        }
        grid = gridCurrent;
    }
    
    for (int i = 0; i < gridDimCurrent.x; ++i)
    {
        for (int j = 0; j < gridDimCurrent.y; ++j)
        {
            for (int k = 0; k < gridDimCurrent.z; ++k)
            {
                const int idx = k + (int)(j * gridDimCurrent.z) + (int)(i * gridDimCurrent.z * gridDimCurrent.y);
                grid[idx] = 0;
                const glm::vec3 center = gridMinCurrent +                           // start at grid min
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

// Image-based fitness

ImageFitnessEval::ImageFitnessEval() : fbo(-1), rbo(-1), refImage(nullptr), currImage(nullptr),
                                       width(400), height(400), sp("src/Shaders/flat.vert", "src/Shaders/flat.frag") {
    // setup framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // create texture attachment
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set texture to be color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // RBO setup
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Use RBO as depth stencil attachment
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // something went wrong
        std::cout << "GL Framebuffer not complete" << std::endl;
    }
}
ImageFitnessEval::~ImageFitnessEval() {
    glDeleteFramebuffers(1, &fbo);
    delete refImage;
    delete currImage;
}

void ImageFitnessEval::SetRefImage(const std::string& path) {
    if (!refImage) {
        const int size = width * height * 4 * sizeof(GLubyte);
        refImage = new GLubyte[size];
    }
    
    int n = 4;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &n, 0);
    if (!data) {
        std::cout << "couldn't load image" << std::endl;
    }

    // copy data to refImage
    for (int i = 0; i < width * height * 4; i++) {
        refImage[i] = (GLuint)data[i];
    }
    // test that this worked
    //int result = stbi_write_png("./res/images/output/test.png", width, height, 4, refImage, width * 4);

    stbi_image_free(data);
}

void ImageFitnessEval::SetCurrImage(Mesh& mesh, const glm::mat4& viewProj, const glm::mat4& model) {
    mesh.Create();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (!currImage) {
        const int size = width * height * 4 * sizeof(GLubyte);
        currImage = new GLubyte[size];
    }

    // Draw ops
    sp.setCameraViewProj("cameraViewProj", viewProj);
    sp.SetModelMatrix("model", model);
    sp.Draw(mesh);

    // Read the framebuffer data into the image
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, currImage);
    /*char buf[25];
    memset(buf, '\0', 25);
    sprintf_s(buf, "%ld", ctr++);
    int result = stbi_write_png((std::string("./res/images/output/YTree_curr") + buf + std::string(".png")).c_str(), width, height, 4, currImage, width * 4);
    */

    mesh.destroy();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int ImageFitnessEval::Evaluate() const {
    int score = 0;
    for (int i = 0; i < width * height * 4; i += 4) {
        if (refImage[i] == currImage[i]) {
            score++;
        }
        else {
            score--;
        }
    }
    return score;
}
