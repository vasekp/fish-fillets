precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform float uTime;

varying vec2 vSrcCoords;

float noise(float x, float y) {
    return fract(sin(mod(x + 1.618 * y, 6.28)) * 100.0);
}

void main(void)
{
    gl_FragColor = noise(vSrcCoords.x, vSrcCoords.y) < (1.5 - uTime)
        ? texture2D(uSrcTexture, vSrcCoords / uSrcSize)
        : vec4(0.0);
}