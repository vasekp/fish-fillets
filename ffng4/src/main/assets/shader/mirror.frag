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
    vec4 lTexColor = texture2D(uSrcTexture, vPixCoords / uSrcSize) * 2.0;
    gl_FragColor = mix(
        lTexColor,
        texture2D(uDstTexture, lPixMirrored / uDstSize),
        smoothstep(1.0, 2.0, lTexColor.a));
}