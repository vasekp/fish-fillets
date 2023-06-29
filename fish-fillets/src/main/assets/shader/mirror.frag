precision highp float;

uniform sampler2D uSrcTexture;
uniform sampler2D uMaskTexture;
uniform vec2 uSrcSize;

varying vec2 vSrcCoords;
varying vec2 vDstCoords;

void main(void)
{
    vec2 lPixMirrored = vSrcCoords / uSrcSize;
    lPixMirrored.x = 1.0 - lPixMirrored.x;
    vec4 lTexColor = texture2D(uSrcTexture, vSrcCoords / uSrcSize) * 2.0;
    gl_FragColor = mix(
        lTexColor,
        texture2D(uMaskTexture, lPixMirrored),
        smoothstep(1.0, 2.0, lTexColor.a));
}
