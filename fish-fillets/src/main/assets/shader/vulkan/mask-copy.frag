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
    vec4 uMaskColor;
} pc;

layout(set = 0, binding = 0) uniform sampler2D uSrcTexture;
layout(set = 1, binding = 0) uniform sampler2D uMaskTexture;

void main() {
    vec2 lTexCoords = vSrcCoords / pc.uSrcSize;
    fragColor = distance(texture(uMaskTexture, lTexCoords), pc.uMaskColor) < 0.1 ? texture(uSrcTexture, lTexCoords) : vec4(0.0);
}
