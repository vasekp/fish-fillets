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
    vec4 uColor1;
    vec4 uColor2;
    float uPeriod;
    float uOffset;
} pc;

layout(binding = 0) uniform sampler2D uSrcTexture;

void main() {
    float alpha = texture(uSrcTexture, vSrcCoords / pc.uSrcSize).a;
    fragColor = alpha * (fract((vSrcCoords.y + pc.uOffset) / pc.uPeriod) < 0.5 ? pc.uColor1 : pc.uColor2);
}
