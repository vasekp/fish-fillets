precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;

varying vec2 vPixCoords;

void main(void)
{
    vec4 ret = texture2D(uSrcTexture, vPixCoords / uSrcSize);
    gl_FragColor = vec4(ret.xyz, step(1.0, ret.w));
}