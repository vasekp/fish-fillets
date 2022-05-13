precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform vec4 uColor1;
uniform vec4 uColor2;
uniform float uTime;

varying vec2 vPixCoords;

void main(void)
{
    float p = min(uTime - vPixCoords.x / uSrcSize.x, 1.0);
    float dy = (1.0 - p) * cos(3.5 * 3.1416 * p) * .5;
    gl_FragColor = texture2D(uSrcTexture, vPixCoords / uSrcSize - vec2(0.0, 1.0 + dy)) * mix(uColor1, uColor2, smoothstep(0.3, 0.8, vPixCoords.y / uSrcSize.y));
}