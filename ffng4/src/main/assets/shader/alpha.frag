precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform float uAlpha;

varying vec2 vPixCoords;

void main(void)
{
    gl_FragColor = texture2D(uSrcTexture, vPixCoords / uSrcSize) * vec4(1.0, 1.0, 1.0, uAlpha);
}