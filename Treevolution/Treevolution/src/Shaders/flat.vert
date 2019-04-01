#version 330 core

/*layout (location = 0)*/ in vec3 vsPos;
//layout (location = 0) out vec3 fsPos;

uniform mat4 cameraViewProj;
uniform mat4 model;

void main() {
    //fsPos = vsPos;
    gl_Position = cameraViewProj * model * vec4(vsPos, 1);
}
