precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform float uAlpha;

varying vec2 vSrcCoords;

void main(void)
{
    gl_FragColor = texture2D(uSrcTexture, vSrcCoords / uSrcSize) * vec4(1.0, 1.0, 1.0, uAlpha);
}