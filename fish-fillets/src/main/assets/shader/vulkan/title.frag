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
    vec2 frac = vec2(vSrcCoords.x - pc.uArea.x / 2.0, pc.uArea.y - vSrcCoords.y);
    float radius = 500.0;
    vec2 rel = vec2(frac.x, radius - frac.y) / radius;
    vec2 polar = vec2(atan(rel.x, rel.y), length(rel));
    vec2 texCoords = vec2(polar.x * radius / pc.uSrcSize.x + 0.5, 1.0 - (1.0 - polar.y) * radius / pc.uSrcSize.y);
    if(clamp(texCoords, vec2(0.0, 0.0), vec2(1.0, 1.0)) != texCoords)
        fragColor = vec4(0.0, 0.0, 0.0, 0.0);
    else
        fragColor = pc.uColor * texture(uSrcTexture, texCoords);
}
