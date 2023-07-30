
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
} pc;

layout(set = 0, binding = 0) uniform sampler2D texY;
layout(set = 1, binding = 0) uniform sampler2D texCb;
layout(set = 2, binding = 0) uniform sampler2D texCr;

void main() {
    vec2 lTexCoords = vSrcCoords / pc.uSrcSize;
    vec3 ycbcr = vec3(texture(texY, lTexCoords).r, texture(texCb, lTexCoords).r, texture(texCr, lTexCoords).r);
    const mat4 mx = mat4(298.082, 298.082, 298.082, 0.0,
            0.0, -100.291, 516.412, 0.0,
            408.583, -208.120, 0.0, 0.0,
            -222.921, 135.576, -276.836, 256.0) / 256.0;
    fragColor = mx * vec4(ycbcr, 1.0);
}
