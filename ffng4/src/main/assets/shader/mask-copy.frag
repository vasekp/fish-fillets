precision highp float;

uniform sampler2D uSrcTexture;
uniform sampler2D uMaskTexture;
uniform vec2 uSrcSize;
uniform vec4 uMaskColor;

varying vec2 vSrcCoords;

void main(void)
{
    vec2 lTexCoords = vSrcCoords / uSrcSize;
    gl_FragColor = distance(texture2D(uMaskTexture, lTexCoords), uMaskColor) < 0.1 ? texture2D(uSrcTexture, lTexCoords) : vec4(0.0);
}