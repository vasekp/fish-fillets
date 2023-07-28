precision highp float;

uniform vec2 uDirection;
uniform float uSize;
uniform vec2 uDstSize;
uniform vec2 uPosition;
uniform float uSign;
attribute vec3 aPosition;
varying vec3 vCoords;

void main() {
    const mat3 triangle = mat3(
        -0.577, -0.33, 0.0,
        0.577, -0.33, 0.0,
        0.0, 0.66, 0.0
    );
    mat2 matrix = mat2(
        uDirection.y, -uDirection.x,
        uDirection.x, uDirection.y
    );
    vCoords = aPosition;
    vec2 xy = uPosition + uSize * matrix * (triangle * uSign * aPosition + vec3(0.0, 1.33, 0.0)).xy;
    vec2 clip = xy / uDstSize * 2.0 - vec2(1.0);
    gl_Position = vec4(clip, 0.0, 1.0);
}
