#version 120

varying vec3 Color;

//layout(location =0) out vec4 FragColor;

void main(){
    gl_FragColor = vec4(Color,1.0);
}
