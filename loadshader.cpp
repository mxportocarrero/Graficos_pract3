#include "loadshader.h"

//Changing the chess to 12 x 10
float n_a = 4, n_b = 2;
float a_sq = n_a/8.0, b_sq = n_b/8.0;

// FORMATO DE LOS VERTICES COLOR | WORLD_COOR | VERTEX_NORMAL | UV_COOR(texture)
VertexData vertices[12] = {
        {{ 0.0f, 1.0f, 0.0f, 1.0f }, { 5.0f,0.0f,8.0f },  {0.0,1.0,0.0},{1.0*(1.0+b_sq),1.0*(1.0+a_sq)}}, // x-z plane 6 vertices
        {{ 0.0f, 1.0f, 0.0f, 1.0f }, { 5.0f,0.0f,-4.0f }, {0.0,1.0,0.0},{1.0*(1.0+b_sq),0.0*(1.0+a_sq)}},
        {{ 0.0f, 1.0f, 0.0f, 1.0f }, { -5.0f,0.0f,-4.0f }, {0.0,1.0,0.0},{0.0*(1.0+b_sq),0.0*(1.0+a_sq)}},

        {{ 0.0f, 1.0f, 0.0f, 1.0f }, { -5.0f,0.0f,-4.0f },{0.0,1.0,0.0},{0.0*(1.0+b_sq),0.0*(1.0+a_sq)}},
        {{ 0.0f, 1.0f, 0.0f, 1.0f }, { -5.0f,0.0f,8.0f },{0.0,1.0,0.0},{0.0*(1.0+b_sq),1.0*(1.0+a_sq)}},
        {{ 0.0f, 1.0f, 0.0f, 1.0f }, { 5.0f,0.0f,8.0f },{0.0,1.0,0.0},{1.0*(1.0+b_sq),1.0*(1.0+a_sq)}},

        {{ 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f,0.0f,0.0f },  {0.0,1.0,0.0},{0.0,0.0}}, // X -axis
        {{ 1.0f, 0.0f, 0.0f, 1.0f }, { 10.0f,0.0f,0.0f }, {0.0,1.0,0.0},{0.0,0.0}},
        {{ 1.0f, 0.0f, 1.0f, 1.0f }, { 0.0f,0.0f,0.0f },  {0.0,1.0,0.0},{0.0,0.0}}, // Y -axis
        {{ 1.0f, 0.0f, 1.0f, 1.0f }, { 0.0f,10.0f,0.0f }, {0.0,1.0,0.0},{0.0,0.0}},
        {{ 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f,0.0f,0.0f },  {0.0,1.0,0.0},{0.0,0.0}}, // Z -axis
        {{ 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f,0.0f,10.0f }, {0.0,1.0,0.0},{0.0,0.0}}
    };

//---------------------------------------------------------------------
//
// Uniform Register Functions
//

void uniformRegister(mat4& var,GLuint shaderID,const char * string){
    GLuint ID = glGetUniformLocation(shaderID,string);
    glUniformMatrix4fv(ID,1,GL_FALSE,&var[0][0]);
}
void uniformRegister(mat3& var,GLuint shaderID,const char * string){
    GLuint ID = glGetUniformLocation(shaderID,string);
    glUniformMatrix3fv(ID,1,GL_FALSE,&var[0][0]);
}
void uniformRegister(vec4 var,GLuint shaderID,const char * string){
    GLuint ID = glGetUniformLocation(shaderID,string);
    glUniform4f(ID, var.x, var.y, var.z, var.w);
}
void uniformRegister(vec3 var,GLuint shaderID,const char * string){
    GLuint ID = glGetUniformLocation(shaderID,string);
    glUniform3f(ID, var.x, var.y, var.z);
}
void uniformRegister(GLfloat var,GLuint shaderID,const char * string){
    GLuint ID = glGetUniformLocation(shaderID,string);
    glUniform1f(ID, var);
}
void uniformRegister(GLint var,GLuint shaderID,const char * string){
    GLuint ID = glGetUniformLocation(shaderID,string);
    glUniform1i(ID, var);
}
void printMatrix(mat4 matrix){
    for (int i = 0; i < 4; i++){
        cout << glm::to_string(vec4(matrix[i])) << endl;
    }
}


//---------------------------------------------------------------------
//
// LoadShaders Function
//

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile)
{
    FILE* fp = fopen(shaderFile, "r");

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);

    buf[size] = '\0';
    fclose(fp);

    return buf;
}


// Create a GLSL program object from vertex and fragment shader files
GLuint LoadShaders(const char* vShaderFile, const char* fShaderFile)
{
    struct Shader {
    const char*  filename;
    GLenum       type;
    GLchar*      source;
    }  shaders[2] = {
    { vShaderFile, GL_VERTEX_SHADER, NULL },
    { fShaderFile, GL_FRAGMENT_SHADER, NULL }
    };

    GLuint program = glCreateProgram();

    for ( int i = 0; i < 2; ++i ) {
    Shader& s = shaders[i];
    s.source = readShaderSource( s.filename );
    if ( shaders[i].source == NULL ) {
        std::cerr << "Failed to read " << s.filename << std::endl;
        exit( EXIT_FAILURE );
       }
        else printf("Successfully read %s\n", s.filename);

    GLuint shader = glCreateShader( s.type );
    glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
    glCompileShader( shader );

    GLint  compiled;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
    if ( !compiled ) {
        std::cerr << s.filename << " failed to compile:" << std::endl;
        GLint  logSize;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
        char* logMsg = new char[logSize];
        glGetShaderInfoLog( shader, logSize, NULL, logMsg );
        std::cerr << logMsg << std::endl;
        delete [] logMsg;

        exit( EXIT_FAILURE );
       }
        else printf("Successfully compiled %s\n", s.filename);

    delete [] s.source;

    glAttachShader( program, shader );
    }

    /* link and error check */
    glLinkProgram(program);

    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
    std::cerr << "Shader program failed to link" << std::endl;
    GLint  logSize;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
    char* logMsg = new char[logSize];
    glGetProgramInfoLog( program, logSize, NULL, logMsg );
    std::cerr << logMsg << std::endl;
    delete [] logMsg;

    exit( EXIT_FAILURE );
    }
    else printf("Successfully linked program object\n\n");

#if 0 /* YJC: Do NOT use this program obj yet!
              Call glUseProgram() outside, in suitable places inside display(),
              to apply different shading programs on different objects.
      */
    /* use program object */
    glUseProgram(program);
#endif

    return program;
}