precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;

varying vec2 vSrcCoords;

void main(void)
{
    vec2 lTexCoords = vSrcCoords / uSrcSize;
    lTexCoords.x = 1.0 - lTexCoords.x;
    gl_FragColor = texture2D(uSrcTexture, lTexCoords);
}