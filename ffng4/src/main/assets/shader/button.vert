precision highp float;

uniform float uSize;
uniform vec2 uPosition;
uniform vec2 uSrcSize;
uniform vec2 uDstSize;
attribute vec2 aPosition;
varying vec2 vCoords;
varying vec2 vTexCoords;

void main() {
    const float padding = 0.2;
    const float fontScale = 0.9;
    vCoords = (2.0 * aPosition - vec2(1.0)) * (1.0 + padding);
    vTexCoords = vec2(uSrcSize.y / fontScale * vCoords.x / 2.0 + uSrcSize.x / 2.0, uSrcSize.y * (vCoords.y / fontScale + 1.0) / 2.0);
    vec2 xy = uPosition + uSize * aPosition.xy;
    vec2 clip = xy / uDstSize * 2.0 - vec2(1.0);
    clip.y = -clip.y;
    gl_Position = vec4(clip, 0.0, 1.0);
}