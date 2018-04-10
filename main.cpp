/**
PRACTICA NRO - 2: Animaciones y efectos con Shaders
Autor: Max W. Portocarrero
Compile Instructions
g++ sphere.cpp loadshader.cpp main.cpp -o app -lGL -lglut -lGLEW -lGLU -std=c++11 -w
**/

#include "sphere.h"
#include "loadshader.h"
#include "texmap.h"


/**VARIABLES GLOBALES**/
const char *filename;
Sphere *sphere;

/**Variables OpenGL*/
enum VAO_IDs{mySphere,extras,NumVAOs};
enum Buffer_IDs{ArrayBuffer,NormalBuffer,extraBuffer,NumBuffers};
enum Texture_IDs{GroundTex,NumTextures};
enum Attrib_IDs{vPos = 0,nPos = 1};
enum eAttrib_IDs{eColor = 0, ePos = 1,eNormal = 2};

GLuint program1,program2,program3,program4,curr_Program;       /* shader program object id */


/**VARIABLES PARA LA ANIMACION**/
enum Path_Points{A,B,C};
enum Paths_IDs{A_to_B,B_to_C,C_to_A}; int currentPath = 0;
vec3 Paths[3] = {vec3(3.0f,1.0f,5.0f),
            vec3(-1.0f,1.0f,-4.0f),
            vec3(3.5f,1.0f,-2.5f),
            };
float t = 0.0f;
float angle = 0.0f;

/**VARIABLES PARA ILUMINACION**/
mat4 Model,View,Projection,MVP,VP,M,V;
mat4 ModelViewMatrix;
mat3 NormalMatrix,NormalMatrix2;

/**LIGHT INFO**/
vec3 AmbientLight = vec3(1.0,1.0,1.0);

LightProperty Lights[2] = {
    { //Light 0 Direccional
        vec4(0.1,0.0,-1.0,0.0),
        vec3(0.8,0.8,0.8),
        vec3(0.2,0.2,0.2),
        vec3(0.0f, 0.0f, 0.0f),
        vec4(0.0,0.0,0.0,0.0)
    },
    { //Light 1 Posicional
        vec4(-14.0, 12.0, -3.0, 1.0),
        vec3(1.0,1.0,1.0),
        vec3(1.0,1.0,1.0),
        vec3(0.0f, 0.0f, 0.0f),
        vec4(-6.0, 0.0, -4.5, 1.0),
    },
};
vec4 LightPos0,LightPos1;//Handles for updating Lights

/** - MATERIAL PROPERTIES - **/
MaterialProperty Mats[NumVAOs] = {
    { //Material 1 Sfera
        vec3(1.0,0.84,0.0),
        vec3(1.0,0.84,1.0),
        vec3(0.2f, 0.2f, 0.2f)
    },
    { //Material 2 Plano
        vec3(0.0,1.0,0.0),
        vec3(0.0,0.0,0.0),
        vec3(0.2f, 0.2f, 0.2f)
    }
};

GLfloat Shininess = 125.f;

/** - FOG PROPERTIES - **/
enum foggy{no_fog,linear,exponential,exponential_square};
int fogType = no_fog;
FogProperty Fog = {
    18.0f,
    0.0f,
    0.09f,
    vec4(0.7, 0.7, 0.7, 0.5)
};

bool addGroundTexture = false;

//==================================
//==================================


/**VARIABLES PARA LOS MENUS**/
enum perNormal{perVertexNormal,perFaceNormal};
enum SmoothFlat{SmoothShading,FlatShading};
bool isWireframeEnabled = false;
int isLightEnabled = 1;
bool isIdling = false;
int _x=0,_y=0,_z=0;


GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint Textures[NumTextures];
bool ShadingType = FlatShading;
bool NormalType = perVertexNormal;

/**----MVP()-----**/
void MVP_handle(){
    /**EVERY GLM::VEC3 USE 3 FLOATS**/
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[ArrayBuffer]);
    glBufferData(GL_ARRAY_BUFFER, 3 * sphere->num_Vertex() * sizeof(float),sphere->data(),GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,Buffers[NormalBuffer]);
    glBufferData(GL_ARRAY_BUFFER, 3 * sphere->num_Vertex() * sizeof(float),sphere->normal_data(NormalType),GL_DYNAMIC_DRAW);

    //Combinación afín para el desplazamiento
    vec3 displacement = (1.0f - t) * Paths[A] + t * Paths[B];
    vec3 dirVectors[3] = {
        Paths[B]-Paths[A],
        Paths[C]-Paths[B],
        Paths[A]-Paths[C],
    };

    float dirAngles[3] = { atan2(dirVectors[A_to_B].z , dirVectors[A_to_B].x) * 180.0 /PI,
                            atan2(dirVectors[B_to_C].z , dirVectors[B_to_C].x)* 180.0 /PI,
                            atan2(dirVectors[C_to_A].z , dirVectors[C_to_A].x)* 180.0 /PI};

    mat4 extraRotation;
    switch(currentPath){
        case A_to_B:
            displacement = (1.0f - t) * Paths[A] + t * Paths[B];
            extraRotation = glm::rotate(mat4(),dirAngles[A_to_B] ,vec3(0.0,1.0,0.0));
            break;
        case B_to_C:
            displacement = (1.0f - t) * Paths[B] + t * Paths[C];
            extraRotation = glm::rotate(mat4(),dirAngles[B_to_C],vec3(0.0,-1.0,0.0));
            break;
        case C_to_A:
            displacement = (1.0f - t) * Paths[C] + t * Paths[A];
            extraRotation = glm::rotate(mat4(),dirAngles[C_to_A],vec3(0.0,-1.0,0.0));
            break;
        default:
            break;
    }

    //cout << displacement.x << "y";
    //Model Matrix: Nuestro Modelo estara en el centro
    //mat4 Model = mat4(1.0f); //Matriz identidad
    Model = glm::translate(mat4(),displacement) *
            //extraRotation *
            glm::rotate(mat4(),angle,vec3(1.0,0.0,0.0));

    //Camera matrix
    View = glm::lookAt(
        vec3(7+_x,3+_y,-10+_z), //Posicion de la Camara en el World Space
        vec3(0,0,0), //Mira hacia el Origen
        vec3(0,1,0) //VUP hacia arriba paralelo al eje y
        );

    //Matriz de Proyección: Campo de vision 45°, Ratio 4:3, display range: 0.1 unit<->100 units
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    Projection = glm::perspective(glm::radians(50.0f), 4.0f / 4.0f, 0.1f, 100.0f);
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // Multiplicacmos las matrices
    MVP = Projection * View * Model; //Observar el orden de multiplicación
    VP = Projection * View; //Para los objetos de referencia en el centro

    ModelViewMatrix = View * Model; // Usado para setear los puntos de luz
    NormalMatrix = mat3(transpose(inverse(View * Model)));
    NormalMatrix2 = mat3(transpose(inverse(View)));
    //NormalMatrix = mat3( vec3(ModelViewMatrix[0]), vec3(ModelViewMatrix[1]), vec3(ModelViewMatrix[2]));
}


/**----INIT()-----*/
void init(){
    //Color por Defecto Background
    glClearColor(0.529f, 0.807f, 0.92f, 0.0f); // cOLOR EXPECIFICADO
    //glClearColor(0.0f,0.0f,0.0f,0.0f);
    glEnable(GL_DEPTH_TEST);

    program1 = LoadShaders("simple.vshader","simple.fshader");
    program2 = LoadShaders("perFragment.vshader","perFragment.fshader");
    program3 = LoadShaders("texture.vshader","texture.fshader");
    program4 = LoadShaders("extra.vshader","extra.fshader");

    //Setting the image for Textures
    image_set_up();
    // La imagen guardada se encuentra en los Handles
    // Image ---> imagen en Buffer Matricial
    // stripeImage ---> imagen guardada en Buffer Lineal

    glGenVertexArrays(NumVAOs,VAOs);
    glBindVertexArray(VAOs[mySphere]);

    //Generamos nuestros buffers
    glGenBuffers(NumBuffers,Buffers);

    //Funciones adicionales
    MVP_handle();

    //Aqui dibujaremos los ejes y el plano
    /**----EXTRAS()-----**/
    glBindVertexArray(VAOs[extras]);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[extraBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_DYNAMIC_DRAW);

    // <--- TEXTURES --->
    glActiveTexture(GL_TEXTURE0);

    glGenTextures(NumTextures,Textures);
    glBindTexture(GL_TEXTURE_2D, Textures[GroundTex]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,ImageWidth,ImageHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,Image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void registerVariables(GLuint program){
    uniformRegister(MVP,program,"MVP");
    uniformRegister(ModelViewMatrix,program,"ModelViewMatrix");
    uniformRegister(NormalMatrix,program,"NormalMatrix");

    uniformRegister(AmbientLight,program,"AmbientLight");

    uniformRegister(LightPos0,program,"Lights[0].Position");

    uniformRegister(Lights[0].Ld,program,"Lights[0].Ld");
    uniformRegister(Lights[0].La,program,"Lights[0].La");
    uniformRegister(Lights[0].Ls,program,"Lights[0].Ls");

    uniformRegister(LightPos1,program,"Lights[1].Position");

    uniformRegister(Lights[1].Ld,program,"Lights[1].Ld");
    uniformRegister(Lights[1].La,program,"Lights[1].La");
    uniformRegister(Lights[1].Ls,program,"Lights[1].Ls");

    uniformRegister(Mats[mySphere].Kd,program,"Material.Kd");
    uniformRegister(Mats[mySphere].Ks,program,"Material.Ks");
    uniformRegister(Mats[mySphere].Ka,program,"Material.Ka");
    uniformRegister(Shininess,program,"Material.Shininess");

    uniformRegister(Fog.maxDist, program , "Fog.maxDist");
    uniformRegister(Fog.minDist, program, "Fog.minDist");
    uniformRegister(Fog.density, program , "Fog.density");
    uniformRegister(Fog.color, program,"Fog.color");

    uniformRegister(fogType,program,"Fog_Type");

    uniformRegister(isLightEnabled,program,"isLightEnabled");
}


/**----DISPLAY()-----**/
void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLineWidth(1.0);

    MVP_handle();

    /**VAO 1**/

    glBindVertexArray(VAOs[mySphere]);
    if(ShadingType){
        glUseProgram(program1);
        curr_Program = program1;
    }
    else{
        glUseProgram(program2);
        curr_Program = program2;
    }

    cout << "light: "<< isLightEnabled << endl;
    //REGISTRAMOS LAS MATRICES PARA PROGRAM1
    //REGISTRAMOS LOS PARAMETROS DE LA LUZ
    LightPos0 = View * Lights[0].Position; //OJO EL ULTIMO NUMERO DEBE SER 1.0
    LightPos1 = View * Lights[1].Position;
    // PORQUE 1.0 EN EL CUARTO VALOR REPRESENTA UNA POSICION EN EL ESPACIO
    // SI FUESE 0.0 REPRESENTARIA UNA DIRECCION

    registerVariables(curr_Program); //<------- Registra las variables en el Shader

    glBindBuffer(GL_ARRAY_BUFFER,Buffers[ArrayBuffer]);
    glEnableVertexAttribArray(vPos);
    glVertexAttribPointer(vPos,3,GL_FLOAT,GL_FALSE,sizeof(vec3),BUFFER_OFFSET(0));

    glBindBuffer(GL_ARRAY_BUFFER,Buffers[NormalBuffer]);
    glEnableVertexAttribArray(nPos);
    glVertexAttribPointer(nPos,3,GL_FLOAT,GL_FALSE,sizeof(vec3),BUFFER_OFFSET(0));

    if(isWireframeEnabled)
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); // Funcion para activar wireframe

        glDrawArrays(GL_TRIANGLES,0,sphere->num_Vertex());

    glDisableVertexAttribArray(vPos);
    glDisableVertexAttribArray(nPos);

    /*====================================================*/
    /*====================================================*/

    /**inter**/
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    /**VAO 2**/

    glBindVertexArray(VAOs[extras]);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[extraBuffer]);

    //DIBUJANDO EL PISO
    if(addGroundTexture){
        glUseProgram(program3);
        curr_Program = program3;
        registerVariables(curr_Program); //<------- Registra las variables en el Shader
    }

    uniformRegister(VP,curr_Program,"MVP");// PEQUEÑO TRUCO
    uniformRegister(View,curr_Program,"ModelViewMatrix");
    uniformRegister(NormalMatrix2,curr_Program,"NormalMatrix");

    uniformRegister(Mats[extras].Kd,curr_Program,"Material.Kd");
    uniformRegister(Mats[extras].Ks,curr_Program,"Material.Ks");
    uniformRegister(Mats[extras].Ka,curr_Program,"Material.Ka");

    //REGISTRO DE LA TEXTURA
    uniformRegister(GroundTex,program3,"Tex1"); // ESTE ES ESPECIAL POR QUE REGISTRA EL SAMPĹER

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, sizeof(VertexData),BUFFER_OFFSET(sizeof(vertices[0].color)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE,
                          sizeof(VertexData),BUFFER_OFFSET( sizeof(vertices[0].color) + sizeof(vertices[0].position) ));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,
        sizeof(VertexData),BUFFER_OFFSET(sizeof( vertices[0].color) + sizeof(vertices[0].position) + sizeof(vertices[0].normal) ));

        glDrawArrays(GL_TRIANGLES,0,6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // DIBUJANDO LOS EJES
    glUseProgram(program4);
    uniformRegister(VP,program4,"VP");

    glEnableVertexAttribArray(eColor);
    glVertexAttribPointer(eColor,3,GL_FLOAT, GL_FALSE, sizeof(VertexData),BUFFER_OFFSET(0));

    glEnableVertexAttribArray(ePos);
    glVertexAttribPointer(ePos,3,GL_FLOAT, GL_FALSE,
                          sizeof(VertexData),BUFFER_OFFSET( sizeof(vertices[0].color) ));

        glLineWidth(1.5);
        glDrawArrays(GL_LINES,6,8);

    glDisableVertexAttribArray(eColor);
    glDisableVertexAttribArray(ePos);

    glutSwapBuffers();

    glFlush();
}

/**FUNCIONES GLUT**/
//---------------------------------------------------------------------------
void idle (void)
{
    angle += 0.01;
    t += 0.001;
    if(t > 1.0f){
        currentPath++;
        t = 0.0f;
        if(currentPath > C_to_A){
            currentPath = A_to_B;
            angle = 0.0;
        }

    }

    //cout << "angle:" << angle << endl;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y){
    switch(key) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit(EXIT_SUCCESS );
	    break;
    case 'b': case 'B':
	    if(!isIdling){
            glutIdleFunc(idle);
            isIdling = true;
	    }else{
            glutIdleFunc(NULL);
            isIdling = false;
        }
        break;
     //LOS VALORES DE X Y Z DEBEN CAMBIAR LA POSICION DE LA CAMARA
    case 'x':
        _x--;
        break;
    case 'X':
	    _x++;
	    break;
    case 'y':
        _y--;
        break;
    case 'Y':
	    _y++;
	    break;
    case 'z':
        _z--;
        break;
    case 'Z':
	    _z++;
	    break;
    }
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        exit(0);
}

/**FUNCIONES MENU**/

void menu(int id){
    switch(id){
        case 1:
            _x=0;_y=0;_z=0;
            break;

        case 2:
            glutIdleFunc(idle);
            break;
        case 3:
            glutIdleFunc(idle);
            break;
    }
    glutPostRedisplay();
}

void Lighting_menu(int id){
    switch(id){
        case 1:
            isLightEnabled = 0;
            break;
        case 2:
            isLightEnabled = 1;
            break;
    }
    glutPostRedisplay();
}

void Wireframe_menu(int id){
    switch(id){
        case 1:
            isWireframeEnabled = 0;
            break;

        case 2:
            isWireframeEnabled = 1;
            break;
    }
    glutPostRedisplay();
}
void Shading_menu(int id){
    switch(id){
        case 1:
            ShadingType = FlatShading;
            break;
        case 2:
            ShadingType = SmoothShading;
            break;
        default:
            break;
    }
    glutPostRedisplay();

}
void Normal_menu(int id){
    switch(id){
        case 1:
            NormalType = perVertexNormal;

            break;
        case 2:
            NormalType = perFaceNormal;
            break;
    }
    glutPostRedisplay();
}
void Fog_menu(int id){
    switch(id){
        case 1:
            fogType = no_fog;
            break;
        case 2:
            fogType = linear;
            break;
        case 3:
            fogType = exponential;
            break;
        case 4:
            fogType = exponential_square;
            break;
    }
    glutPostRedisplay();
}
void Texture_menu(int id){
    switch(id){
        case 1:
            addGroundTexture = false;
            break;

        case 2:
            addGroundTexture = true;
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char ** argv){
    filename = argv[1];
    sphere = new Sphere(filename);


    /**Inicio de OpenGL**/
    glutInit(&argc,argv);

    glewExperimental = GL_TRUE;

    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(windowDIM,windowDIM);
    //glutInitContextVersion(3,0);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow(filename);

    if(glewInit()){
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }

    init();

    /**MENU**/
    int sub_menu1 = glutCreateMenu(Lighting_menu);
    glutAddMenuEntry("No",1);
    glutAddMenuEntry("Yes",2);

    int sub_menu2 = glutCreateMenu(Wireframe_menu);
    glutAddMenuEntry("No",1);
    glutAddMenuEntry("Yes",2);

    int sub_menu3 = glutCreateMenu(Shading_menu);
    glutAddMenuEntry("Flat Shading",1);
    glutAddMenuEntry("Smooth Shading",2);

    int sub_menu4 = glutCreateMenu(Normal_menu);
    glutAddMenuEntry("Per Vertex Normal",1);
    glutAddMenuEntry("Per Face Normal",2);

    int sub_menu5 = glutCreateMenu(Fog_menu);
    glutAddMenuEntry("No Fog",1);
    glutAddMenuEntry("linear",2);
    glutAddMenuEntry("Exponential",3);
    glutAddMenuEntry("Exponential Square",4);

    int sub_menu6 = glutCreateMenu(Texture_menu);
    glutAddMenuEntry("No",1);
    glutAddMenuEntry("Yes",2);

    glutCreateMenu(menu);
    glutAddMenuEntry("Default View Point",1);
    glutAddSubMenu("Enable Lighting",sub_menu1);
    glutAddSubMenu("Shading",sub_menu3);
    glutAddSubMenu("Normal Type",sub_menu4);
    glutAddSubMenu("Wire Frame",sub_menu2);
    glutAddSubMenu("Fog",sub_menu5);
    glutAddSubMenu("Texture",sub_menu6);
    glutAddMenuEntry("Quit",6);
    glutAttachMenu(GLUT_RIGHT_BUTTON);


    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(myMouse);


    glutMainLoop();
}
