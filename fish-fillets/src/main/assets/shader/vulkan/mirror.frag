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
    vec2 lPixMirrored = vSrcCoords / pc.uSrcSize;
    lPixMirrored.x = 1.0 - lPixMirrored.x;
    vec4 lTexColor = texture(uSrcTexture, vSrcCoords / pc.uSrcSize) * 2.0;
    fragColor = mix(
        lTexColor,
        texture(uMaskTexture, lPixMirrored),
        smoothstep(1.0, 2.0, lTexColor.a));
}
