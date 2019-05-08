#version 330 core
out vec4 FragColor;

uniform vec4 vertexColor;

void main() {
	FragColor = vec4(vertexColor);
}
