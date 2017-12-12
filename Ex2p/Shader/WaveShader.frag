varying vec3 normal;

void main()
{
    float intensity;
    vec4 color;
    vec3 n = normalize(normal);
    intensity = dot(normalize(vec3(gl_LightSource[0].position)),n);

    gl_FragColor = vec4(1.0, 0.5, 0.5, 1);

}