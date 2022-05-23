precision highp float;

uniform vec4 uColor;
uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
varying vec2 vCoords;
varying vec2 vTexCoords;

void main(void)
{
    vec2 pv = pow(abs(vCoords), vec2(5.0));
    float q = pv.x + pv.y;
    gl_FragColor = step(q, 1.0) * uColor * (1.0 - texture2D(uSrcTexture, vTexCoords / uSrcSize).a);
}