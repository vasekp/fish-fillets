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
    float uTime;
} pc;

layout(binding = 0) uniform sampler2D uSrcTexture;

void main() {
    const float fullWidth = 640.0;
    const float freq = 3.5;
    const float ampl = 0.5;
    float p = min(pc.uTime - vSrcCoords.x / fullWidth, 1.0);
    float dy = (1.0 - p) * cos(freq * 3.1416 * p) * ampl;
    vec2 texCoords = vSrcCoords * pc.uCoords.z / pc.uSrcSize - vec2(0.0, 1.0 + dy);
    fragColor = texture(uSrcTexture, texCoords) * mix(pc.uColor1, pc.uColor2, smoothstep(0.3, 0.8, texCoords.y));
}
