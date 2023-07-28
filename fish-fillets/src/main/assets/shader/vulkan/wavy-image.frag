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
    float uAmplitude;
    float uPeriod;
    float uPhase;
} pc;

layout(binding = 0) uniform sampler2D uSrcTexture;

void main() {
    float dx = pc.uAmplitude * sin(vSrcCoords.y / pc.uPeriod + pc.uPhase);
    fragColor = texture(uSrcTexture, (vSrcCoords + vec2(dx, 0.0)) / pc.uSrcSize);
}
