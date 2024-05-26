#include <shader.h>
#include <utils.h>

#include <fstream>
#include <sstream>

Shader::Shader(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
  init(vsPath, fsPath, gsPath);
}

void Shader::init(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
  if (gsPath != "") {
    initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath), getCodeFromFile(gsPath));
  }
  else {
    initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath), gsPath);
  }
}

///
/// \brief Init shader with shader codes
///
/// \param vs The vertex shader code content
/// \param fs The fragment shader code content
///
/// \todo Implement this function to set shaders to OpenGL
///
void Shader::initWithCode(const std::string &vs, const std::string &fs, const std::string &gs) {
  // UNIMPLEMENTED;
  const char* vShaderCode = vs.c_str();
  const char* fShaderCode = fs.c_str();
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  // get vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  // check vertex shader compile status
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILE_FAILED\n" << infoLog << std::endl;
  }

  // get fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  // check fragment shader compile status
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILE_FAILED\n" << infoLog << std::endl;
  }

  unsigned int geometry;
  if (gs != "") {
      const char* gShaderCode = gs.c_str();
      geometry = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(geometry, 1, &gShaderCode, NULL);
      glCompileShader(geometry);
      // check fragment shader compile status
      glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
      if(!success)
      {
          glGetShaderInfoLog(geometry, 512, NULL, infoLog);
          std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
      };
  }  

  // create shader program
  id_ = glCreateProgram();
  glAttachShader(id_, vertex);
  glAttachShader(id_, fragment);
  if (gs != "") {
    glAttachShader(id_, geometry);
  }
  glLinkProgram(id_);
  // check shader program link status
  glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id_, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
  }
  // delete shader
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (gs != "") {
    glDeleteShader(geometry);
  }
}

std::string Shader::getCodeFromFile(const std::string &path) {
  std::string code;
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(getPath(path));
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  } catch (std::ifstream::failure &e) {
    LOG_ERR("File Error: " + std::string(e.what()));
  }
  return code;
}

void Shader::use() const { glUseProgram(id_); }

GLint Shader::getUniformLocation(const std::string &name) const {
  return glGetUniformLocation(id_, name.c_str());
}

void Shader::setInt(const std::string &name, GLint value) const {
  glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, GLfloat value) const {
  glUniform1f(getUniformLocation(name), value);
}

void Shader::setMat3(const std::string &name, const mat3 &value) const {
  glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const mat4 &value) const {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, const vec3 &value) const {
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const vec4 &value) const {
  glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec2(const std::string &name, const vec2 &value) const {
  glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
