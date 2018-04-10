#ifndef INCLUDES_H
#define INCLUDES_H

/**Core LIB**/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::fstream;
using std::cerr;

/**OPENGL LIB**/

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

typedef glm::vec3 vec3;
typedef glm::mat4 mat4;
typedef glm::mat3 mat3;
typedef glm::vec4 vec4;


#define BUFFER_OFFSET(a) ((void*)(a))
#define windowDIM 600

#define PI 3.14159265




#endif // INCLUDES_H
