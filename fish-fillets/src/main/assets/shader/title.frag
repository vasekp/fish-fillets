precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSizeScaled;
uniform vec2 uBlitSize;
uniform vec4 uColor;

varying vec2 vSrcCoords;

void main(void) {
    vec2 frac = vec2(vSrcCoords.x - uBlitSize.x / 2.0, uBlitSize.y - vSrcCoords.y);
    float radius = 500.0;
    vec2 rel = vec2(frac.x, radius - frac.y) / radius;
    vec2 polar = vec2(atan(rel.x, rel.y), length(rel));
    vec2 texCoords = vec2(polar.x * radius / uSrcSizeScaled.x + 0.5, 1.0 - (1.0 - polar.y) * radius / uSrcSizeScaled.y);
    gl_FragColor = uColor * texture2D(uSrcTexture, texCoords);
}