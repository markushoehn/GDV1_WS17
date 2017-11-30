// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: shader functions                                                 //
// ========================================================================= //

#include <stdio.h>        // cout
#include <iostream>       // cout
#include <fstream>        // read file

using namespace std;

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::cout << infoLog << std::endl;
		free(infoLog);
	}
}

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::cout << infoLog << std::endl;
		free(infoLog);
	}
}

GLuint readShaders(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
  // read vertex shader if possible
  std::ifstream vin(vertexShaderFilename, std::ifstream::in | std::ios::binary);
  vin.seekg(0, std::ios::end);
  int lengthV = vin.tellg();
  vin.seekg(0, std::ios::beg); 
  if (lengthV < 0) { std::cout << "readShaders(): " << vertexShaderFilename << " not found!" << std::endl; return 0; }
  char *vsource = new char[lengthV+1];
  vin.read(vsource, lengthV);
  vin.close();
  vsource[lengthV] = 0; // terminate char array
  // read fragment shader if possible
  std::ifstream fin(fragmentShaderFilename, std::ifstream::in | std::ios::binary);
  fin.seekg(0, std::ios::end);
  int lengthF = fin.tellg();
  fin.seekg(0, std::ios::beg);
  if (lengthF < 0) { std::cout << "readShaders(): " << fragmentShaderFilename << " not found!" << std::endl; return 0; }
  char *fsource = new char[lengthF+1];
  fin.read(fsource, lengthF);
  fin.close();
  fsource[lengthF] = 0; // terminate char array
  // create shaders, set source and compile
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vertexShader, 1, (const char**)&vsource, 0);
  glShaderSource(fragmentShader, 1, (const char **)&fsource, 0);
  glCompileShader(vertexShader);
  glCompileShader(fragmentShader);
  // create a program, attach both shaders and link the program (shaders can be deleted now)
  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);  
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);  
  // check of all was successfull and return the programID
  GLint success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    cout << "===== Vertex Shader =====" << endl << vsource << endl;
    cout << endl;
    cout << "===== Vertex Shader Info Log =====" << endl;
    printShaderInfoLog(vertexShader);
    cout << endl;
    cout << "===== Fragment Shader =====" << endl << fsource << endl;
    cout << endl;
    cout << "===== Fragment Shader Info Log =====" << endl;
    printShaderInfoLog(fragmentShader);
    cout << endl;
    cout << "===== Program Info Log =====" << endl;
    printProgramInfoLog(program);
    cout << endl;
    glDeleteProgram(program);
    program = 0;
  }
  delete[] vsource;
  delete[] fsource;
  return program;
}