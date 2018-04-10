#version 120

attribute vec3 VertexColor;
attribute vec3 VertexPosition;

uniform mat4 VP;

varying vec3 Color;

void main(){
    Color = VertexColor;
    gl_Position = VP * vec4(VertexPosition,1);
}
