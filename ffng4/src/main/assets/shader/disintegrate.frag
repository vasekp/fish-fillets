precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform float uTime;

varying vec2 vSrcCoords;

float alpha0(float x, float y) {
    return step(fract(sin(mod(x + 1.618 * y, 6.28)) * 100.0), 1.5 - uTime);
}

float alpha(float x, float y) {
  return mix(
      mix(alpha0(floor(x), floor(y)), alpha0(ceil(x), floor(y)), fract(x)),
      mix(alpha0(floor(x), ceil(y)), alpha0(ceil(x), ceil(y)), fract(x)),
      fract(y));
}

void main(void)
{
    gl_FragColor = texture2D(uSrcTexture, vSrcCoords / uSrcSize) * alpha(vSrcCoords.x, vSrcCoords.y);
}
