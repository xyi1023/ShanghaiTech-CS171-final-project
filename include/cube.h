#ifndef CUBE_H_
#define CUBE_H_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "defines.h"
#include "utils.h"

class Cube {
private:
    GLfloat vertices[24];
    GLuint indices[36];

public:
    explicit Cube();
    void create(GLuint &VAO, GLuint &VBO, GLuint &EBO) const;
};

#endif // CUBE_H_