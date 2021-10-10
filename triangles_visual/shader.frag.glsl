#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in float intencity;
layout(location = 2) in vec3 light_color;


layout(location = 0) out vec4 outColor;

void main() {
    vec3 final = mix(fragColor, light_color, 0.5*intencity);
    outColor = vec4(final, 1.0);
}

