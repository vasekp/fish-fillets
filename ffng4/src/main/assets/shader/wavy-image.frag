precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform float uAmplitude;
uniform float uPhase;
uniform float uPeriod;

varying vec2 vPixCoords;

void main(void)
{
    float dx = uAmplitude * sin(vPixCoords.y / uPeriod + uPhase);
    gl_FragColor = texture2D(uSrcTexture, (vPixCoords + vec2(dx, 0.0)) / uSrcSize);
}