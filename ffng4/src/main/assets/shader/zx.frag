precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform vec4 uColor1;
uniform vec4 uColor2;
uniform float uPeriod;
uniform float uOffset;

varying vec2 vSrcCoords;

void main(void)
{
    float alpha = texture2D(uSrcTexture, vSrcCoords / uSrcSize).a;
    gl_FragColor = alpha * (fract((vSrcCoords.y + uOffset) / uPeriod) < 0.5 ? uColor1 : uColor2);
}