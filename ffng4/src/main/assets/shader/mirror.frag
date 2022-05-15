precision highp float;

uniform sampler2D uSrcTexture;
uniform sampler2D uMaskTexture;
uniform vec2 uDstOffset;
uniform vec2 uDstSize;
uniform vec2 uSrcOffset;
uniform vec2 uSrcSize;

varying vec2 vPixCoords;
varying vec2 vDstCoords;

void main(void)
{
    vec2 lPixMirrored = vec2(2.0 * uSrcOffset.x - vPixCoords.x + 3.0, vPixCoords.y) + vec2(3.0, 0.0);
    lPixMirrored.y = uSrcSize.y - lPixMirrored.y; // OpenGL coordinate reversal
    vec4 lTexColor = texture2D(uMaskTexture, vDstCoords / uDstSize) * 2.0;
    gl_FragColor = mix(
        lTexColor,
        texture2D(uSrcTexture, lPixMirrored / uSrcSize),
        smoothstep(1.0, 2.0, lTexColor.a));
}