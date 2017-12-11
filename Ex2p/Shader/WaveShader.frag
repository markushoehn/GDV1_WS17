varying vec3 normal;

void main()
{
    float intensity;
    vec4 color;
    vec3 n = normalize(normal);
    intensity = dot(vec3(gl_LightSource[0].position),n);

    gl_FragColor = vec4(vec3(normal * intensity), 1);

}