precision highp float;

uniform vec4 uColor;
uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform vec2 uTexSize;
varying vec2 vSrcCoords;

void main(void) {
    vec2 coords = vSrcCoords / uSrcSize * 2.0 - vec2(1.0, 1.0);
    vec2 texCoords = vSrcCoords - uSrcSize / 2.0 + uTexSize / 2.0;
    vec2 pv = pow(abs(coords), vec2(5.0));
    float q = pv.x + pv.y;
    gl_FragColor = step(q, 1.0) * uColor * (1.0 - texture2D(uSrcTexture, texCoords / uTexSize).a);
}
