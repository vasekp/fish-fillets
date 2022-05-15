precision highp float;

uniform sampler2D uSrcTexture;
uniform sampler2D uDstTexture;
uniform vec2 uDstOffset;
uniform vec2 uSrcSize;
uniform vec2 uDstSize;

varying vec2 vPixCoords;

void main(void)
{
    vec2 lPixMirrored = uDstOffset + vec2(-1.0, 1.0) * vPixCoords + vec2(3.0, 0.0);
    lPixMirrored.y = uDstSize.y - lPixMirrored.y; // OpenGL coordinate reversal
    vec4 lTexColor = texture2D(uSrcTexture, vPixCoords / uSrcSize);
    vec4 lMaskColor = texture2D(uSrcTexture, vec2(0.5, 0.5));
    gl_FragColor = mix(
        texture2D(uDstTexture, lPixMirrored / uDstSize),
        lTexColor,
        clamp(distance(lTexColor, lMaskColor), 0.0, 1.0));
}