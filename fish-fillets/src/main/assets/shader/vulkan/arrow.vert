#version 450

layout(location = 0) out vec3 vCoords;

layout(push_constant) uniform constants {
    vec2 uSrcOffset;
    vec2 uSrcSize;
    vec2 uDstOffset;
    vec2 uDstSize;
    vec2 uArea;
    vec4 uCoords;
    vec2 uPosition;
    vec2 uDirection;
    float uSize;
    float uSign;
    vec4 uColor;
} pc;

const mat3 triangle = mat3(
    -0.577, -0.33, 0.0,
    0.577, -0.33, 0.0,
    0.0, 0.66, 0.0
);

const vec3 vertices[3] = vec3[](
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1)
);

void main() {
    mat2 matrix = mat2(
        pc.uDirection.y, -pc.uDirection.x,
        pc.uDirection.x, pc.uDirection.y
    );
    vec3 aPosition = vertices[gl_VertexIndex];
    vCoords = aPosition;
    vec2 xy = pc.uPosition + pc.uSize * matrix * (triangle * pc.uSign * aPosition + vec3(0.0, 1.33, 0.0)).xy;
    vec2 clip = xy / pc.uDstSize * 2.0 - vec2(1.0);
    gl_Position = vec4(clip, 0.0, 1.0);
}
