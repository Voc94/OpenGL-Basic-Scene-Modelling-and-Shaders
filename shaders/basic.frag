#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
out vec4 fColor;
in vec4 fPosLightSpace; // Position in light space
// Shadow map texture
uniform sampler2D shadowMap; 
// Point Light 1 uniform
uniform vec3 pointLightColor1;
uniform vec3 pointLightPosEye1;
uniform float pointLightAmbientStrength1;
uniform float pointLightSpecularStrength1;
// Point Light 2 uniform
uniform vec3 pointLightColor2;
uniform vec3 pointLightPosEye2;
uniform float pointLightAmbientStrength2;
uniform float pointLightSpecularStrength2;
// Sun (Directional Light)
uniform vec3 sunLightColor;
uniform vec3 sunLightDirection;
uniform float sunLightAmbientStrength;
uniform float sunLightSpecularStrength;
//transperancy
uniform float alpha;

uniform sampler2D diffuseTexture;
in vec2 fragTexCoords;

float shininess1 = 32.0f;
float constant1 = 1.0f;
float linear1 = 0.022f;
float quadratic1 = 0.007f;

float shininess2 = 64.0f;
float constant2 = 1.0f;
float linear2 = 0.014f;
float quadratic2 = 0.007f;
float computeShadow() {
    vec3 projCoords = fPosLightSpace.xyz / fPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0,1] range
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    return shadow;
}
void main() {
    // Ambient
	// Ambient Point 1
    vec3 ambientPoint1 = pointLightAmbientStrength1 * pointLightColor1;
	// Ambient Point 2
	vec3 ambientPoint2 = pointLightAmbientStrength2 * pointLightColor2;
	// Ambient Sun
    vec3 ambientSun = sunLightAmbientStrength * sunLightColor;

    // Diffuse Lighting
    vec3 norm = normalize(fNormal);
	// Diffuse Point 1
    vec3 lightDirPoint1 = normalize(pointLightPosEye1 - fPosEye.xyz);
    float diffPoint1 = max(dot(norm, lightDirPoint1), 0.0);
    vec3 diffusePoint1 = diffPoint1 * pointLightColor1;
	// Diffuse Point 2
	vec3 lightDirPoint2 = normalize(pointLightPosEye2 - fPosEye.xyz);
    float diffPoint2 = max(dot(norm, lightDirPoint2), 0.0);
    vec3 diffusePoint2 = diffPoint2 * pointLightColor2;
	// Diffuse Sun
    vec3 lightDirSun = normalize(-sunLightDirection);
    float diffSun = max(dot(norm, lightDirSun), 0.0);
    vec3 diffuseSun = diffSun * sunLightColor;

    // Specular
    vec3 viewDir = normalize(-fPosEye.xyz);
	//Specular Point 1
    vec3 reflectDirPoint1 = reflect(-lightDirPoint1, norm);
    float specPoint1 = pow(max(dot(viewDir, reflectDirPoint1), 0.0), shininess1);
    vec3 specularPoint1 = pointLightSpecularStrength1 * specPoint1 * pointLightColor1;
	//Specular Point 2
	vec3 reflectDirPoint2 = reflect(-lightDirPoint2, norm);
    float specPoint2 = pow(max(dot(viewDir, reflectDirPoint2), 0.0), shininess2);
    vec3 specularPoint2 = pointLightSpecularStrength2 * specPoint2 * pointLightColor2;
	//Specular Sun
    vec3 reflectDirSun = reflect(-lightDirSun, norm);
    float specSun = pow(max(dot(viewDir, reflectDirSun), 0.0), shininess1);
    vec3 specularSun = sunLightSpecularStrength * specSun * sunLightColor;

    // Attenuation for Point Light 1
    float distance1 = length(pointLightPosEye1 - fPosEye.xyz);
    float attenuation1 = 1.0 / (constant1 + linear1 * distance1 + quadratic1 * distance1 * distance1);
	ambientPoint1 *= attenuation1;
    diffusePoint1 *= attenuation1;
    specularPoint1 *= attenuation1;
	// Attenuation for Point Light 2
	float distance2 = length(pointLightPosEye2 - fPosEye.xyz);
    float attenuation2 = 1.0 / (constant2 + linear2 * distance2 + quadratic2 * distance2 * distance2);
	ambientPoint2 *= attenuation2;
    diffusePoint2 *= attenuation2;
    specularPoint2 *= attenuation2;
	// Combine results
     float shadow = computeShadow(); // Compute shadow value
    vec3 result = (ambientPoint1 + (1.0 - shadow) * diffusePoint1 + (1.0 - shadow) * specularPoint1) + (ambientPoint2 + (1.0 - shadow) * diffusePoint2 + (1.0 - shadow) * specularPoint2) +(ambientSun + (1.0 - shadow) * diffuseSun + (1.0 - shadow) * specularSun);
    vec4 textureColor = texture(diffuseTexture, fragTexCoords);
    fColor = vec4(result * textureColor.rgb, alpha * textureColor.a);

}
