precision highp float;

uniform float uSize;
uniform vec2 uDstSize;
uniform vec2 uPosition;
attribute vec2 aPosition;
varying vec2 vCoords;

void main() {
    const float padding = 0.2;
    vCoords = (2.0 * aPosition - vec2(1.0)) * (1.0 + padding);
    vec2 xy = uPosition + uSize * aPosition.xy;
    vec2 clip = xy / uDstSize * 2.0 - vec2(1.0);
    clip.y = -clip.y;
    gl_Position = vec4(clip, 0.0, 1.0);
}