#version 120

varying vec3 LightIntensity;

//layout(location = 0) out vec4 FragColor;

struct MaterialInfo{
    vec3 Ka; // Reflectividad Ambiental
    vec3 Kd; // Difusa
    vec3 Ks; // Especular
    float Shininess; // Factor de brillo
};

uniform MaterialInfo Material;

uniform int isLightEnabled;

void main(){
        gl_FragColor = vec4(LightIntensity,1.0);

    	//FragColor = vec4(Material.Kd,1.0);
}
