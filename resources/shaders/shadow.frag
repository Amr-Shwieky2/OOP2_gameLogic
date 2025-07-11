#version 120

// Light parameters
uniform vec2 u_resolution;
uniform vec2 u_lightPosition;
uniform vec3 u_lightColor;
uniform float u_lightIntensity;
uniform float u_lightRadius;

// Soft shadow parameters
uniform float u_shadowSoftness = 32.0;
uniform float u_darknessLevel = 0.92;

void main() {
    // Normalized coordinates
    vec2 texCoord = gl_TexCoord[0].xy;
    
    // Calculate distance from current pixel to light source
    vec2 pixelPosition = texCoord * u_resolution;
    float distance = length(pixelPosition - u_lightPosition);
    
    // Calculate light attenuation (falloff)
    float attenuation = 1.0 - smoothstep(0.0, u_lightRadius, distance);
    
    // Apply soft edges to the light
    attenuation = pow(attenuation, 1.5);
    
    // Apply light color and intensity
    vec3 finalColor = vec3(u_lightColor) * attenuation * u_lightIntensity;
    
    // Output final color
    gl_FragColor = vec4(finalColor, attenuation);
}