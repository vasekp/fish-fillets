#version 450

layout(location = 0) out vec2 vSrcCoords;
layout(location = 1) out vec2 vDstCoords;

layout(push_constant) uniform constants {
    vec2 uSrcOffset;
    vec2 uSrcSize;
    vec2 uDstOffset;
    vec2 uDstSize;
    vec3 uCoords;
    vec2 uSigns;
} pc;

const vec2 rect[4] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0)
);

void main() {
  vec2 aPosition = rect[gl_VertexIndex] * pc.uSrcSize;
  vSrcCoords = pc.uSrcOffset + aPosition;
  vDstCoords = (pc.uDstOffset + aPosition) * pc.uCoords.z + pc.uCoords.xy;
  vec2 lClipCoords = vDstCoords / pc.uDstSize * 2.0 - vec2(1.0, 1.0);
  gl_Position = vec4(pc.uSigns * lClipCoords, 0.0, 1.0);
}
