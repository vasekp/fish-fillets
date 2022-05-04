precision highp float;

attribute vec2 aPosition;

uniform vec2 uSrcOffset;
uniform vec2 uDstOffset;
uniform vec2 uDstSize;

varying vec2 vPixCoords;

void main() {
  vPixCoords = aPosition + uSrcOffset;
  vec2 lClipCoords = (aPosition + uDstOffset) / uDstSize * 2.0 - vec2(1.0, 1.0);
  gl_Position = vec4(lClipCoords.x, -lClipCoords.y, 0.0, 1.0);
}