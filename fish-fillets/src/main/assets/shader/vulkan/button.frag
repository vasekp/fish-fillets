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

layout(binding = 0) uniform sampler2D uSrcTexture;

void main() {
    vec2 coords = vSrcCoords / pc.uArea * 2.0 - vec2(1.0, 1.0);
    vec2 texCoords = vSrcCoords - pc.uArea / 2.0 + pc.uSrcSize / 2.0;
    vec2 pv = pow(abs(coords), vec2(5.0));
    float q = pv.x + pv.y;
    fragColor = step(q, 1.0) * vec4(pc.uColor.rgb * (1.0 - texture(uSrcTexture, texCoords / pc.uSrcSize).a), 1.0);
}
