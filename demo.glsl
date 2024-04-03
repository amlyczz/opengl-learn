#version 410
layout (location = 0) in vec3 aPos;
out vec4 vertexVector;
void main() {
    gl_Position = vec4(aPos, 1.0);
    vertexVector = vec4(0.5, 0.0, 0.0, 1.0);

}

#version 410
out vec4 FragColor;
in vec4 vertexVector;

void main(){
    FragColor = vertexVector;
}