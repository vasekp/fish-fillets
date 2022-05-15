precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform float uAmplitude;
uniform float uPhase;
uniform float uPeriod;

varying vec2 vSrcCoords;

void main(void)
{
    float dx = uAmplitude * sin(vSrcCoords.y / uPeriod + uPhase);
    gl_FragColor = texture2D(uSrcTexture, (vSrcCoords + vec2(dx, 0.0)) / uSrcSize);
}