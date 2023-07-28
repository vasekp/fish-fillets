#version 450

layout(location = 0) in vec2 vSrcCoords;
layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform constants {
    vec2 uSrcOffset;
    vec2 uSrcSize;
    vec2 uDstOffset;
    vec2 uDstSize;
    vec2 uArea;
    vec4 uCoords;
    vec4 uColor;
} pc;

void main() {
    //fragColor = vec4(vSrcCoords / pc.uSrcSize, 1, 1);
    fragColor = pc.uColor;
}
