#include "sphere.h"

Sphere::Sphere(const char* filename){
    src_file = filename;
    load_Data();
}

void Sphere::load_Data(){
    std::fstream file;
    file.open(src_file);
    file >> num_of_Triangles;

    float x,y,z; //Variables que se guardaran
    int tmp;
    vertices = (vec3 *)malloc( num_of_Triangles * 3 * sizeof(vec3));
    //normals = (vec3 *)malloc( num_of_Triangles * 3 * sizeof(vec3));
    normals = (vec3 *)malloc( num_of_Triangles * 3 * sizeof(vec3)); //Dibujando las normales
    flat_normals = (vec3 *)malloc( num_of_Triangles * 3 * sizeof(vec3)); //Dibujando las normales
    for(int i = 0; i < num_of_Triangles*3; i++){
        if(i%3 == 0)
            file >> tmp;
        file >> x >> y >> z;

        vertices[i] = vec3(x,y,z);
        normals[i] = normalize(vec3(x,y,z));
        //normals[i] = vec3(-1.0,0.0,0.0);
    }
    for(int i = 0; i < num_of_Triangles*3; i+=3){
        vec3 fn = normalize(cross( normals[i+1] - normals[i] ,normals[i+2] - normals[i]));// Flat Normal
        flat_normals[i] = fn;
        flat_normals[i+1] = fn;
        flat_normals[i+2] = fn;
    }

}

void Sphere::print_VertexData(){
    cout << "Vertices:\n";
    for(int i = 0 ; i < num_of_Triangles*3; i++){
        cout<<"(" << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << ")\n";
    }
    cout << endl;
}

Sphere::~Sphere(){
    free(vertices);
}






