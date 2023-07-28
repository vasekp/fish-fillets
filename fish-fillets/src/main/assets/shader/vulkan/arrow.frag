#version 450

layout(location = 0) in vec3 vCoords;
layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform constants {
    vec2 uSrcOffset;
    vec2 uSrcSize;
    vec2 uDstOffset;
    vec2 uDstSize;
    vec2 uArea;
    vec4 uCoords;
    vec2 uPosition;
    float uSize;
    vec2 uDirection;
    float uSign;
    vec4 uColor;
} pc;

void main() {
    vec3 pv = sqrt(abs(vCoords));
    float q = pv.x + pv.y + pv.z;
    fragColor = step(1.4, q) * pc.uColor;
}
