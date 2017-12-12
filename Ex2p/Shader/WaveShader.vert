uniform float time;
varying vec3 normal;

void main() {
    vec4 v = vec4(gl_Vertex);
    v.x = sin(5.0*v.z + time)*0.25;

    normal = gl_NormalMatrix * gl_Normal;
    gl_Position = gl_ModelViewProjectionMatrix * v;
}