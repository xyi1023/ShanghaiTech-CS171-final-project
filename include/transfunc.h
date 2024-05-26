#ifndef TRANSFUNC_H_
#define TRANSFUNC_H_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "defines.h"
#include "utils.h"

class TransferFunction {
public:
    TransferFunction(GLuint threshold);
    void setColorMapTexture();
    GLuint getColorMapTexture() const;
    double ExtinctionToMaterialOpacity (float extinction);

private:
    GLuint threshold = 32;
    GLuint tff_texture;
};

#endif // TRANSFUNC_H_