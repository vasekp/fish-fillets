precision highp float;

attribute vec2 aPosition;

uniform vec2 uDisplaySize;
uniform vec2 uTextureSize;
uniform vec2 uCanvasSize;

varying vec2 vRescaled;

void main() {
    vRescaled = (vec2(1.0) - aPosition) / 2.0 / uTextureSize * uCanvasSize;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}