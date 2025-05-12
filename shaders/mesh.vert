#version 450

layout(set = 0, binding = 0) uniform CameraUBO{
    mat4 view;
    mat4 proj;
    uint isOrtho;
} cam;

layout(set = 0, binding = 1) uniform ObjectUBO{
    mat4 model;
}ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

layout (location = 0) out vec2 fragUV;

void main() {
    gl_Position = cam.proj * cam.view * ubo.model * vec4(inPosition, 1.0);
    fragUV = inUV;
}
