precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform vec4 uColor1;
uniform vec4 uColor2;

varying vec2 vPixCoords;

void main(void)
{
    gl_FragColor = texture2D(uSrcTexture, vPixCoords / uSrcSize) * mix(uColor1, uColor2, smoothstep(0.3, 0.8, vPixCoords.y / uSrcSize.y));
}