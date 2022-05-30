precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;

varying vec2 vSrcCoords;

void main(void)
{
    vec4 ret = texture2D(uSrcTexture, vSrcCoords / uSrcSize);
    gl_FragColor = step(1.0, ret.w) * ret;
}