precision highp float;

uniform vec4 uColor;
varying vec2 vCoords;
uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;

void main(void)
{
    vec2 pv = pow(abs(vCoords), vec2(5.0));
    float q = pv.x + pv.y;
    vec2 texCoords = vec2(uSrcSize.y * vCoords.x / 2.0 + uSrcSize.x / 2.0, uSrcSize.y * (vCoords.y + 1.0) / 2.0);
    gl_FragColor = step(q, 1.0) * uColor * (1.0 - texture2D(uSrcTexture, texCoords / uSrcSize).a);
}