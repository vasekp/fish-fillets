precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform vec4 uColor1;
uniform vec4 uColor2;
uniform vec3 uCoords;
uniform float uTime;

varying vec2 vDstCoords;
varying vec2 vSrcCoords;

void main(void)
{
    const float fullWidth = 640.0;
    const float freq = 3.5;
    const float ampl = 0.5;
    float p = min(uTime - vSrcCoords.x / fullWidth, 1.0);
    float dy = (1.0 - p) * cos(freq * 3.1416 * p) * ampl;
    vec2 texCoords = vSrcCoords * uCoords.z / uSrcSize - vec2(0.0, 1.0 + dy);
    gl_FragColor = texture2D(uSrcTexture, texCoords) * mix(uColor1, uColor2, smoothstep(0.3, 0.8, texCoords.y));
}
