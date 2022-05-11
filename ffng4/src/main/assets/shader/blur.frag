precision highp float;

uniform sampler2D uSrcTexture;
uniform vec2 uSrcSize;

varying vec2 vPixCoords;
uniform vec2 uDelta;

void main(void)
{
    vec4 pure = (texture2D(uSrcTexture, vPixCoords / uSrcSize)
        + 0.867 * texture2D(uSrcTexture, (vPixCoords + uDelta) / uSrcSize)
        + 0.565 * texture2D(uSrcTexture, (vPixCoords + 2.0 * uDelta) / uSrcSize)
        + 0.276 * texture2D(uSrcTexture, (vPixCoords + 3.0 * uDelta) / uSrcSize)
        + 0.101 * texture2D(uSrcTexture, (vPixCoords + 4.0 * uDelta) / uSrcSize)
        + 0.028 * texture2D(uSrcTexture, (vPixCoords + 5.0 * uDelta) / uSrcSize)
        + 0.867 * texture2D(uSrcTexture, (vPixCoords - uDelta) / uSrcSize)
        + 0.565 * texture2D(uSrcTexture, (vPixCoords - 2.0 * uDelta) / uSrcSize)
        + 0.276 * texture2D(uSrcTexture, (vPixCoords - 3.0 * uDelta) / uSrcSize)
        + 0.101 * texture2D(uSrcTexture, (vPixCoords - 4.0 * uDelta) / uSrcSize)
        + 0.028 * texture2D(uSrcTexture, (vPixCoords - 5.0 * uDelta) / uSrcSize)
    ) / 4.676;
    float avg = (pure.r + pure.g + pure.b) / 3.0;
    float base = mix(avg, 1.0, 0.3);
    gl_FragColor = vec4(mix(pure.rgb, vec3(base), 0.7), 1.0);
}