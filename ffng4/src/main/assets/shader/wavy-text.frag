precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform vec4 uColor1;
uniform vec4 uColor2;
uniform float uTime;

varying vec2 vSrcCoords;

void main(void)
{
    float p = min(uTime - vSrcCoords.x / uSrcSize.x, 1.0);
    float dy = (1.0 - p) * cos(3.5 * 3.1416 * p) * .25;
    vec2 texCoords = vSrcCoords / uSrcSize - vec2(0.0, 1.0 + dy);
    gl_FragColor = texture2D(uSrcTexture, texCoords) * mix(uColor1, uColor2, smoothstep(0.3, 0.8, texCoords.y));
}