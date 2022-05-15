precision highp float;

attribute vec2 aPosition;

uniform vec2 uSrcOffset;
uniform vec2 uSrcSize;
uniform vec2 uDstOffset;
uniform vec2 uDstSize;
uniform vec2 uSigns;

varying vec2 vPixCoords;
varying vec2 vDstCoords;

void main() {
  vPixCoords = uSrcOffset + (vec2(1.0) - uSigns) / 2.0 * uSrcSize + uSigns * aPosition;
  vDstCoords = uDstOffset + aPosition;
  vec2 lClipCoords = (aPosition + uDstOffset) / uDstSize * 2.0 - vec2(1.0, 1.0);
  gl_Position = vec4(lClipCoords.x, -lClipCoords.y, 0.0, 1.0);
}