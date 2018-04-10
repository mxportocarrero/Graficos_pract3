#ifndef LOADSHADER_H
#define LOADSHADER_H

#include "includes.h"


static char* readShaderSource(const char* shaderFile);
// Create a GLSL program object from vertex and fragment shader files
GLuint LoadShaders(const char* vShaderFile, const char* fShaderFile);

void uniformRegister(mat4& var,GLuint shaderID,const char * string);
void uniformRegister(mat3& var,GLuint shaderID,const char * string);
void uniformRegister(vec4 var,GLuint shaderID,const char * string);
void uniformRegister(vec3 var,GLuint shaderID,const char * string);
void uniformRegister(GLfloat var,GLuint shaderID,const char * string);
void uniformRegister(GLint var,GLuint shaderID,const char * string);
void printMatrix(mat4 matrix);


struct VertexData {
        GLfloat color[4];
        GLfloat position[4];
        GLfloat normal[4];
        GLfloat UV[2];
    };

extern VertexData vertices[12];

struct MaterialProperty{
	vec3 Kd; // Difuso
	vec3 Ks; // Especular
	vec3 Ka; // Ambiental
};

struct LightProperty{
	vec4 Position;
	vec3 Ld; // Difuso
	vec3 Ls; // Especular
	vec3 La; // Ambiental
	vec4 Direction; // Direccion, para SpotLight
};

struct FogProperty{
	GLfloat maxDist;
	GLfloat minDist;
	GLfloat density;
	vec4 color;
};

#endif LOADSHADER_H
