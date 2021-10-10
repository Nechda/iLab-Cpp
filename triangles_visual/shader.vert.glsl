#version 450

layout(binding = 0) uniform Scene_t {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cam_pos;
    vec3 light_dir;
    vec3 light_color;
} scene;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out float intencity;
layout(location = 2) out vec3 light_color;


void main() {
    gl_Position = scene.proj * scene.view * scene.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    light_color = scene.light_color.xyz;


    float diffuse = abs(dot(scene.light_dir.xyz, inNormal));

    vec3 normal = inNormal;
    normal = normalize(normal);
    vec3 to_cam = scene.cam_pos - vec3(scene.model * vec4(inPosition, 1.0));
    to_cam = normalize(to_cam);
    vec3 reflected = scene.light_dir - 2 * dot(scene.light_dir, normal) * normal;
    reflected = normalize(reflected);


    intencity = diffuse;//0.5 * pow(abs(dot(to_cam, reflected)), 4);

}

