precision highp float;

uniform sampler2D uSrcTexture; /* Y */
uniform sampler2D uCbTexture;
uniform sampler2D uCrTexture;
uniform vec2 uSrcSize;

varying vec2 vSrcCoords;

void main(void)
{
    vec2 lTexCoords = vSrcCoords / uSrcSize;
    vec3 ycbcr = vec3(texture2D(uSrcTexture, lTexCoords).r, texture2D(uCbTexture, lTexCoords).r, texture2D(uCrTexture, lTexCoords).r);
    const mat4 mx = mat4(298.082, 298.082, 298.082, 0.0,
            0.0, -100.291, 516.412, 0.0,
            408.583, -208.120, 0.0, 0.0,
            -222.921, 135.576, -276.836, 256.0) / 256.0;
    gl_FragColor = mx * vec4(ycbcr, 1.0);
}
