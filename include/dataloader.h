#ifndef DATALOADER_H_
#define DATALOADER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "defines.h"
#include "utils.h"

class DataLoader {
public:
    GLuint texture, hash_texture, offset_texture;
    DataLoader(std::string path, int x, int y, int z, int datatype, bool hashing);
    void PSH(GLubyte* rawdata);

private:
    int size;
};

#endif