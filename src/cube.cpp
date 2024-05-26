#include <cube.h>
#include <utils.h>
#include <fstream>
#include <vector>
#include <sstream>
#include "defines.h"

Cube::Cube(): vertices{
      0.0, 0.0, 0.0,
      0.0, 0.0, 1.0,
      0.0, 1.0, 0.0,
      0.0, 1.0, 1.0,
      1.0, 0.0, 0.0,
      1.0, 0.0, 1.0,
      1.0, 1.0, 0.0,
      1.0, 1.0, 1.0}, indices{
      1, 5, 7,
      7, 3, 1,
      0, 2, 6,
      6, 4, 0,
      0, 1, 3,
      3, 2, 0,
      7, 5, 4,
      4, 6, 7,
      2, 3, 7,
      7, 6, 2,
      1, 0, 4,
      4, 5, 1
    }{}

void Cube::create(GLuint &VAO, GLuint &VBO, GLuint &EBO) const {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(int), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (float*) NULL);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (float*) NULL);
    glBindVertexArray(0);
}