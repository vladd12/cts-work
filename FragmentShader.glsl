#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    float value = texture(ourTexture, TexCoord).r;
    FragColor = vec4(value, value, value, 1.0);
}