#pragma once
#include "coordsys.h"
#include "entity.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
namespace components
{

class Camera : public Component
{
  public:
    DEFINE_COMPONENT_ID(Camera);
    Camera(float aspect, float fov_degrees)
        : m_aspect{aspect}, m_fov_degrees{fov_degrees}, m_projection_mat{1.0}, m_rot_mat{1}, m_sensitivity(2),
          m_yaw(0.0f), m_pitch(0.0f)
    {
        update_projection_matrix();
    };
    void set_aspect_ratio(float aspect)
    {
        m_aspect = aspect;
        update_projection_matrix();
    }
    void set_fov_degrees(float fov_degrees)
    {
        m_fov_degrees = fov_degrees;
        update_projection_matrix();
    }
    const glm::mat4x4& projection_mat() const
    {
        return m_projection_mat;
    }

    glm::mat4x4 view_mat()
    {
        return glm::lookAt(m_position, m_position + forward(), up());
    }

    void reset()
    {
        m_position = glm::vec3{0, 0, 0};
        m_yaw = 0;
        m_pitch = 0;
        m_rot_mat = glm::mat3x3{1};
    }

    glm::vec3& position()
    {
        return m_position;
    }

    glm::vec3 forward()
    {
        return m_rot_mat[2];
    }
    glm::vec3 right()
    {
        return m_rot_mat[0];
    }
    glm::vec3 up()
    {
        return m_rot_mat[1];
    }

    void set_sensitivity(float sensitivity)
    {
        m_sensitivity = sensitivity;
    }
    float sensitivity()
    {
        return m_sensitivity;
    }

    void rotate(float yaw, float pitch)
    {
        m_yaw += yaw;
        m_pitch += pitch;
        m_rot_mat = glm::eulerAngleYXZ(m_yaw, -m_pitch, 0.0f);
    }

  private:
    void update_projection_matrix()
    {
        m_projection_mat = glm::perspective(glm::radians(m_fov_degrees), m_aspect, 0.1f, 200.0f);
        m_projection_mat[1][1] *= -1;
    }

  private:
    float m_aspect;
    float m_fov_degrees;
    glm::mat4x4 m_projection_mat;

    glm::mat3x3 m_rot_mat;
    glm::vec3 m_position;
    float m_yaw;
    float m_pitch;
    float m_sensitivity;
};
} // namespace components