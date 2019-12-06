#pragma once

#include <iostream>
#include <string>

#include <GLES3/gl3.h>
#include <glm/glm.hpp>

#include "scene.hpp"

class ShaderProgram {
 public:
  ShaderProgram(SceneObject* parent = nullptr)
      : ID(glCreateProgram()), parent_(parent) {}

  ~ShaderProgram() {
    glUseProgram(0);
    glDeleteProgram(ID);
  }

  void setParent(SceneObject* parent) {
    parent_ = parent;
    ensureRendering();
  }

  void attachShader(std::string src, GLenum shaderType) {
    const char* src_c = src.c_str();
    int shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &src_c, NULL);
    glCompileShader(shaderID);
    checkCompileErrors(shaderID, "SHADER");
    glAttachShader(ID, shaderID);
    glDeleteShader(shaderID);
  }

  void link() {
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
  }

  void use() const { glUseProgram(ID); }

  void setBool(const std::string& name, bool value) const {
    use();
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    ensureRendering();
  }
  // ------------------------------------------------------------------------
  void setInt(const std::string& name, int value) const {
    use();
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    ensureRendering();
  }
  // ------------------------------------------------------------------------
  void setFloat(const std::string& name, float value) const {
    use();
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    ensureRendering();
  }
  // ------------------------------------------------------------------------
  void setVec2(const std::string& name, const glm::vec2& value) const {
    use();
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    ensureRendering();
  }
  void setVec2(const std::string& name, float x, float y) const {
    use();
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    ensureRendering();
  }
  // ------------------------------------------------------------------------
  void setVec3(const std::string& name, const glm::vec3& value) const {
    use();
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    ensureRendering();
  }
  void setVec3(const std::string& name, float x, float y, float z) const {
    use();
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    ensureRendering();
  }
  // ------------------------------------------------------------------------
  void setVec4(const std::string& name, const glm::vec4& value) const {
    use();
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    ensureRendering();
  }
  void setVec4(const std::string& name, float x, float y, float z, float w) {
    use();
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    ensureRendering();
  }
  // ------------------------------------------------------------------------
  void setMat2(const std::string& name, const glm::mat2& mat) const {
    use();
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
    ensureRendering();
  }
  // ------------------------------------------------------------------------
  void setMat3(const std::string& name, const glm::mat3& mat) const {
    use();
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
    ensureRendering();
  }
  // ------------------------------------------------------------------------
  void setMat4(const std::string& name, const glm::mat4& mat) const {
    use();
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
    ensureRendering();
  }

  const int ID;

 private:
  ShaderProgram(const ShaderProgram&);
  ShaderProgram& operator=(const ShaderProgram&);

  void ensureRendering() const {
    if (parent_)
      parent_->ensureRendering();
  }

  // utility function for checking shader compilation/linking errors.
  // ------------------------------------------------------------------------
  void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
      }
    } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
      }
    }
  }

 private:
  SceneObject* parent_;
};