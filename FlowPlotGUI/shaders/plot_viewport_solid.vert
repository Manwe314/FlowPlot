#version 450

layout(push_constant) uniform ScenePushConstants {
    float width;
    float height;
    float cycleValue;
    float pad;
} pc;

layout(location = 0) out vec2 vUv;

void main() {
    vec2 positions[6] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0, -1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0,  1.0)
    );

    vec2 position = positions[gl_VertexIndex];
    vUv = position * 0.5 + 0.5;
    gl_Position = vec4(position, 0.0, 1.0);
}
