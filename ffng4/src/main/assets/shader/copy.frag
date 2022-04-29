precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;

varying vec2 vPixCoords;

void main(void)
{
    gl_FragColor = texture2D(uSrcTexture, vPixCoords / uSrcSize);
}