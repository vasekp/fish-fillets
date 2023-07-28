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
    float uAlpha;
} pc;

layout(binding = 0) uniform sampler2D uSrcTexture;

void main() {
    fragColor = pc.uAlpha * texture(uSrcTexture, vSrcCoords / pc.uSrcSize);
}
