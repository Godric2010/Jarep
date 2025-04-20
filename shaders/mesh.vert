#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

layout (location = 0) out vec2 fragUV;

void main() {
    vec3 pos = inPosition - vec3(0.0, 5.0, 10.0);
    gl_Position = vec4(pos * 0.1, 1.0);
    fragUV = inUV;
}
