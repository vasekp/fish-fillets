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
    vec2 uDelta;
} pc;

layout(binding = 0) uniform sampler2D uSrcTexture;

void main() {
    vec4 pure = (texture(uSrcTexture, vSrcCoords / pc.uSrcSize)
        + 0.867 * texture(uSrcTexture, (vSrcCoords + pc.uDelta) / pc.uSrcSize)
        + 0.565 * texture(uSrcTexture, (vSrcCoords + 2.0 * pc.uDelta) / pc.uSrcSize)
        + 0.276 * texture(uSrcTexture, (vSrcCoords + 3.0 * pc.uDelta) / pc.uSrcSize)
        + 0.101 * texture(uSrcTexture, (vSrcCoords + 4.0 * pc.uDelta) / pc.uSrcSize)
        + 0.028 * texture(uSrcTexture, (vSrcCoords + 5.0 * pc.uDelta) / pc.uSrcSize)
        + 0.867 * texture(uSrcTexture, (vSrcCoords - pc.uDelta) / pc.uSrcSize)
        + 0.565 * texture(uSrcTexture, (vSrcCoords - 2.0 * pc.uDelta) / pc.uSrcSize)
        + 0.276 * texture(uSrcTexture, (vSrcCoords - 3.0 * pc.uDelta) / pc.uSrcSize)
        + 0.101 * texture(uSrcTexture, (vSrcCoords - 4.0 * pc.uDelta) / pc.uSrcSize)
        + 0.028 * texture(uSrcTexture, (vSrcCoords - 5.0 * pc.uDelta) / pc.uSrcSize)
    ) / 4.676;
    float avg = (pure.r + pure.g + pure.b) / 3.0;
    float base = mix(avg, 1.0, 0.3);
    fragColor = vec4(mix(pure.rgb, vec3(base), 0.7), 1.0);
}
