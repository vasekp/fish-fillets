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
    float uTime;
} pc;

layout(binding = 0) uniform sampler2D uSrcTexture;

float noise(float x, float y) {
    return fract(sin(mod(x + 1.618 * y, 6.28)) * 100.0);
}

void main() {
    fragColor = noise(vSrcCoords.x, vSrcCoords.y) < (1.5 - pc.uTime)
        ? texture(uSrcTexture, vSrcCoords / pc.uSrcSize)
        : vec4(0.0);
}
