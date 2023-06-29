precision highp float;

attribute vec2 aPosition;

uniform vec2 uSrcOffset;
uniform vec2 uSrcSize;
uniform vec2 uDstOffset;
uniform vec2 uDstSize;
uniform vec3 uCoords;
uniform vec2 uSigns;

varying vec2 vSrcCoords;
varying vec2 vDstCoords;

void main() {
  vSrcCoords = uSrcOffset + aPosition;
  vDstCoords = (uDstOffset + aPosition) * uCoords.z + uCoords.xy;
  vec2 lClipCoords = vDstCoords / uDstSize * 2.0 - vec2(1.0, 1.0);
  gl_Position = vec4(uSigns * lClipCoords, 0.0, 1.0);
}