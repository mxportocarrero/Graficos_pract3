#ifndef SPHERE_H
#define SPHERE_H

#include "includes.h"

class Sphere{
    const char *src_file;
    int num_of_Triangles;
    int num_of_Vertices;
    vec3 *vertices; //Puntero para reservar la memoria y guarda la data de vertices
    vec3 *normals;
    vec3 *flat_normals;

public:
    Sphere(const char* _filename);
    void load_Data();
    void print_VertexData();

    int num_Triangles(){ return num_of_Triangles;}
    int num_Vertex(){return num_of_Triangles * 3;}
    vec3 * data(){return vertices;}
    vec3 * normal_data(bool flat = 0){
        if (flat)
            return flat_normals;
        return normals;
    }

    ~Sphere();
};



#endif // SPHERE_H

