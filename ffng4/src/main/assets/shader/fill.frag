precision highp float;

uniform sampler2D uScreenTexture;

varying vec2 vRescaled;

void main(void)
{
    gl_FragColor = texture2D(uScreenTexture, vRescaled);
}