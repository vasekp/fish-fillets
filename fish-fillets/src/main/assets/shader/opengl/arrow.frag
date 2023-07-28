precision highp float;

uniform vec4 uColor;
varying vec3 vCoords;

void main(void)
{
    vec3 pv = sqrt(abs(vCoords));
    float q = pv.x + pv.y + pv.z;
    gl_FragColor = step(1.4, q) * uColor;
}