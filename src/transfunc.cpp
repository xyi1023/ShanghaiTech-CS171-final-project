#include <transfunc.h>
#include <utils.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <glm/glm.hpp>
#include "defines.h"

TransferFunction::TransferFunction(GLuint thres): threshold(thres) {}

double TransferFunction::ExtinctionToMaterialOpacity (float extinction) {
    return 1.0 - glm::exp(-extinction);
}

void TransferFunction::setColorMapTexture() {
    glGenTextures(1, &tff_texture);
    glBindTexture(GL_TEXTURE_1D, tff_texture);

    // allocate memory for texture
    const int MAX_CNT = 256;
    GLubyte *tff = (GLubyte *)calloc(MAX_CNT * 4, sizeof(GLubyte));

    // set transfer function
    for (int i = 0; i < 256; i++) {
        float ratio = i / 255.0f;
        double exp = ExtinctionToMaterialOpacity(ratio);
        tff[4 * i] = static_cast<GLubyte>(255 * (1.0 - exp)); 
        tff[4 * i + 1] = static_cast<GLubyte>(255 * (1.0 - exp)); 
        tff[4 * i + 2] = static_cast<GLubyte>(255 * (1.0 - exp)); 
        tff[4 * i + 3] = static_cast<GLubyte>(exp * 255);
    }
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, threshold, 0, GL_RGBA, GL_UNSIGNED_BYTE, tff);
    free(tff);
}

GLuint TransferFunction::getColorMapTexture() const {
    return tff_texture;
}