#include <BlackBox/CShader.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

ShaderStatus::ShaderStatus(CShader *shader) :
  m_Shader(shader)
{

}

bool ShaderStatus::get(GLenum statusType) {
  glGetShaderiv(m_Shader->get(), statusType, &m_Status);
  if(m_Status != GL_TRUE)
  {
    glGetShaderInfoLog(m_Shader->get(), 512, NULL, infoLog);
    std::cout << "ERROR: shader" << m_Shader->getName() <<"\n" << infoLog << std::endl;
    return false;
  }
  return true;
 
}

ShaderProgramStatus::ShaderProgramStatus(CShaderProgram *program) :
  m_Program(program)
{

}

bool ShaderProgramStatus::get(GLenum statusType) {
  GLsizei size;
  glGetProgramiv(m_Program->get(), statusType, &m_Status);
  if(m_Status != GL_TRUE)
  {
    glGetProgramInfoLog(m_Program->get(), 512, &size, infoLog);
    std::cout << "ERROR: shader::programm: \n" << infoLog << std::endl;
    return false;
  }
  return true;
 
}
CShader::CShader(string text, CShader::type type) :
  m_Text(text), m_Type(type), m_Status(this) 
{
  
}

CShader::~CShader() {
  glDeleteShader(m_Shader);
}

bool CShader::create() {
  m_Shader = glCreateShader(m_Type);
  if (m_Shader != 0) { return true; }
  else { return false; }
  // return m_Status.get(GL_VALIDATE_STATUS);
}


CShader *CShader::load(string path, CShader::type type) {
  ifstream fin(path);
  string buff;
  string text;

  if (!fin.is_open()) return nullptr;
  
  while (getline(fin, buff)) {
    text += buff;
    text += '\n';
  }

  CShader *shader = new CShader(text, type);
  if (!shader->create())
    return nullptr;
  shader->compile();
  shader->print();
  return shader;
}

bool CShader::compile() {
  cout  << "shader: " << m_Shader << endl;
  const char *text = m_Text.c_str();
  glShaderSource(m_Shader, 1, &text, nullptr);
  cout << "test" << endl;
  glCompileShader(m_Shader);
  return m_Status.get(GL_COMPILE_STATUS);
}

bool CShader::bind() {
 return true; 
}

void CShader::print() {
  cout << m_Text << endl;
}

string CShader::typeToStr()
{
  switch (m_Type) {
  case E_VERTEX:
    return "vertex";
  case E_FRAGMENT:
    return "fragment";
  }
}

string CShader::getName() {
  return m_Path;
}

GLuint CShader::get() {
  return m_Shader;
}

CShaderProgram::CShaderProgram() : m_Status(this)
{
  m_Program = glCreateProgram();
  created = true;
}

CShaderProgram::CShaderProgram(
  CShader *vertex, CShader *fragment) :
  m_Status(this)
{
  m_Vertex = vertex;
  m_Fragment = fragment;
}

CShaderProgram::~CShaderProgram() {

}

bool CShaderProgram::create() {
  if (!created) m_Program = glCreateProgram();
  if (!attached)
  {
    attach(m_Vertex);
    attach(m_Fragment);
  }
  link();
	return m_Status.get(GL_LINK_STATUS);
}

void CShaderProgram::attach(CShader *shader) {
  switch (shader->m_Type) {
  case CShader::type::E_VERTEX:
    if (m_Vertex == nullptr)
      m_Vertex = shader;
    break;
  case CShader::type::E_FRAGMENT:
    if (m_Fragment == nullptr)
      m_Fragment = shader;
    break;
  }
  glAttachShader(m_Program, shader->get());
  attached = true;
}

bool CShaderProgram::link() {
  glLinkProgram(m_Program);
  //delete m_Vertex;
  //delete m_Fragment;
	return m_Status.get(GL_LINK_STATUS);
}

void CShaderProgram::use() {
    glUseProgram(m_Program);
}

void CShaderProgram::unuse()
{
    glUseProgram(0);
}

void CShaderProgram::setUniformValue(int value, const char * format, ...)
{
  va_list ptr;
  va_start(ptr, value);

  char name[128];
  vsprintf(name, format, ptr);

  GLint loc = glGetUniformLocation(m_Program, name);
  if (loc != -1){
      glUniform1i(loc, value);
  }

  va_end(ptr);
}

void CShaderProgram::setUniformValue(float value, const char * format, ...)
{
  va_list ptr;
  va_start(ptr, value);

  char name[128];
  vsprintf(name, format, ptr);

  GLint loc = glGetUniformLocation(m_Program, name);
  if (loc != -1){
        glUniform1f(loc, value);
    }

  va_end(ptr);
}

void CShaderProgram::setUniformValue(glm::vec1 value, const char * format, ...)
{
  va_list ptr;
  va_start(ptr, value);

  char name[128];
  vsprintf(name, format, ptr);

  GLint loc = glGetUniformLocation(m_Program, name);
  if (loc != -1){
        glUniform1f(loc, value[0]);
    }

  va_end(ptr);
}

void CShaderProgram::setUniformValue(glm::vec2 value, const char * format, ...)
{
  va_list ptr;
  va_start(ptr, value);

  char name[128];
  vsprintf(name, format, ptr);

  GLint loc = glGetUniformLocation(m_Program, name);
  if (loc != -1){
        glUniform2fv(loc, 1, glm::value_ptr(value));
    }

  va_end(ptr);
}

void CShaderProgram::setUniformValue(glm::vec3 value, const char * format, ...)
{
  va_list ptr;
  va_start(ptr, format);

  char name[128];
  vsprintf(name, format, ptr);

  GLint loc = glGetUniformLocation(m_Program, name);
  if (loc != -1){
      glUniform3fv(loc, 1, glm::value_ptr(value));
  }
  int er = glGetError();

  va_end(ptr);
}

void CShaderProgram::setUniformValue(glm::vec4 value, const char * format, ...)
{
  va_list ptr;
  va_start(ptr, value);

  char name[128];
  vsprintf(name, format, ptr);

  GLint loc = glGetUniformLocation(m_Program, name);
  if (loc != -1){
        glUniform4fv(loc, 1, glm::value_ptr(value));
    }

  va_end(ptr);
}

void CShaderProgram::setUniformValue(glm::mat2 value, const char * format, ...)
{
  va_list ptr;
  va_start(ptr, value);

  char name[128];
  vsprintf(name, format, ptr);

  GLint loc = glGetUniformLocation(m_Program, name);
  if (loc != -1){
    glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(value));
  }

  va_end(ptr);
}

void CShaderProgram::setUniformValue(glm::mat3 value, const char * format, ...)
{
  va_list ptr;
  va_start(ptr, value);

  char name[128];
  vsprintf(name, format, ptr);

  GLint loc = glGetUniformLocation(m_Program, name);
  if (loc != -1){
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
  }

  va_end(ptr);
}

void CShaderProgram::setUniformValue(glm::mat4 value, const char * format, ...)
{
  va_list ptr;
  va_start(ptr, value);

  char name[128];
  vsprintf(name, format, ptr);

  GLint loc = glGetUniformLocation(m_Program, name);
  if (loc != -1){
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
  }

  va_end(ptr);
}

GLuint CShaderProgram::get() {
  return m_Program;
}
