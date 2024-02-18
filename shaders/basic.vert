#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fNormal;
out vec4 fPosEye;
out vec4 fPosLightSpace; // Output position in light space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceTrMatrix; // Light space transformation matrix
uniform mat3 normalMatrix;

out vec2 fragTexCoords;

void main() {
    fragTexCoords = vTexCoords;
    fPosEye = view * model * vec4(vPosition, 1.0f);
    fNormal = normalize(normalMatrix * vNormal);
    fPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f); // Transform position to light space
    gl_Position = projection * view * model * vec4(vPosition, 1.0f);
}
