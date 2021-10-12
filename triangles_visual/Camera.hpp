#pragma once

#include "Lowering/Window.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

struct Camera {

    Camera() {
        dir_ = glm::vec3(1, 1, 1);
        dir_ = glm::normalize(dir_);
        dist_ = 10.0f;
    }

    void update() {
        using glm::radians;
        using glm::vec2;
        using glm::vec3;

        const float mouse_factor = 0.07;

        vec2 mouse_pos = get_mouse_pos();
        vec2 mouse_delta = mouse_pos - old_mouse_pos;
        old_mouse_pos = mouse_pos;

        mouse_delta *= -1.0f;

        static bool is_first_launch = true;
        if (is_first_launch) {
            is_first_launch = 0;
            return;
        }

        dist_ += get_delta_dist();
        dist_ = glm::clamp(dist_, 20.0f, 250.0f);

        phi -= mouse_factor * mouse_delta.x;
        theta -= mouse_factor * mouse_delta.y;
        theta = glm::clamp(theta, -89.0f, 89.0f);
        float r_phi = glm::radians(phi);
        float r_theta = glm::radians(theta);
        dir_ = glm::vec3(cos(r_theta) * sin(r_phi), cos(r_theta) * cos(r_phi), sin(r_theta));
    }

    glm::vec3 get_position() { return dir_ * dist_; }

private:
    glm::vec3 dir_;
    float dist_;
    float phi = 0;
    float theta = 0;
    glm::vec2 old_mouse_pos;

    glm::vec2 get_mouse_pos() {
        const WindowInfo &winfo = WindowInfo::Instance();
        double xm, ym;
        glfwGetCursorPos(winfo.descriptor, &xm, &ym);
        return glm::vec2(xm, ym);
    }

    float get_delta_dist() {
        const WindowInfo &winfo = WindowInfo::Instance();
        bool inc = glfwGetKey(winfo.descriptor, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS;
        bool dec = glfwGetKey(winfo.descriptor, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS;
        const float factor = 0.1;
        return factor * (inc * 1.0f + dec * (-1.0f));
    }
};
