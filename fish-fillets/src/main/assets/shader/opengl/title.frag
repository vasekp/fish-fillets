precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;
uniform vec2 uArea;
uniform vec4 uColor;

varying vec2 vSrcCoords;

void main(void) {
    vec2 frac = vec2(vSrcCoords.x - uArea.x / 2.0, uArea.y - vSrcCoords.y);
    float radius = 500.0;
    vec2 rel = vec2(frac.x, radius - frac.y) / radius;
    vec2 polar = vec2(atan(rel.x, rel.y), length(rel));
    vec2 texCoords = vec2(polar.x * radius / uSrcSize.x + 0.5, 1.0 - (1.0 - polar.y) * radius / uSrcSize.y);
    if(clamp(texCoords, vec2(0.0, 0.0), vec2(1.0, 1.0)) != texCoords)
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    else
        gl_FragColor = uColor * texture2D(uSrcTexture, texCoords);
}
