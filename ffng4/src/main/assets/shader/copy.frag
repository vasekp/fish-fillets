precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;

varying vec2 vPixCoords;

void main(void)
{
    gl_FragColor = mix(texture2D(uSrcTexture, vPixCoords / uSrcSize), vec4(1), 0.5);
}