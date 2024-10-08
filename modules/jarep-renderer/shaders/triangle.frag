#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragUV;
layout (set = 1, binding = 2) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor *texture(texSampler, fragUV).rgb, 1.0);//vec4(fragColor, 1.0);
}
