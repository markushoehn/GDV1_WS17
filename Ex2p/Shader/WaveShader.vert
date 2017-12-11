varying vec3 normal;
uniform float time;

void main() {
    vec4 v = vec4(gl_Vertex);
    v.z = sin(5.0 * v.x + time * 0.05) * 0.25;

    normal = gl_NormalMatrix * gl_Normal;
    gl_Position = gl_ModelViewProjectionMatrix * v;
}