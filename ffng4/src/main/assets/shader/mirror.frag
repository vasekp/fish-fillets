precision highp float;

uniform sampler2D uSrcTexture;
uniform sampler2D uDstTexture;
uniform vec2 uDstOffset;
uniform vec2 uSrcSize;
uniform vec2 uDstSize;
uniform float uBorder;

varying vec2 vPixCoords;

void main(void)
{
    vec2 lPixMirrored = uDstOffset + vec2(-1.0, 1.0) * vPixCoords + vec2(uBorder, 0.0);
    vec2 lTexCoords = vPixCoords / uSrcSize;
    vec4 lTexColor = texture2D(uSrcTexture, lTexCoords);
    vec4 lMaskColor = texture2D(uSrcTexture, vec2(0.5, 0.5));
    gl_FragColor = distance(lTexColor, lMaskColor) < 0.1
    ? texture2D(uDstTexture, lPixMirrored / uDstSize)
    : lTexColor;
}