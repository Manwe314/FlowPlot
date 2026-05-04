#version 450

layout(push_constant) uniform ScenePushConstants {
    float width;
    float height;
    float cycleValue;
    float pad;
} pc;

layout(location = 0) in vec2 vUv;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0, 0.0, 1.0, 1.0);
}
