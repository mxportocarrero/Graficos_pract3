#version 430 core

layout(location = 1) in vec3 VertexPosition;
layout(location = 0) in vec3 VertexColor;

uniform mat4 VP;

out vec3 Color;

void main(){
    Color = VertexColor;
    gl_Position = VP * vec4(VertexPosition,1);
}
