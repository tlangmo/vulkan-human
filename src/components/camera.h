#pragma once
#include "entity.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace rendersystem
{

class CameraComponent : public Component
{
  public:
    DEFINE_COMPONENT_ID(CameraComponent);
    CameraComponent(float aspect, float fov_degrees)
        : m_aspect{aspect}, m_fov_degrees{fov_degrees}, m_projection_mat{1.0}, m_view_mat{1.0}
    {
        m_projection_mat = glm::perspective(glm::radians(m_fov_degrees), m_aspect, 0.1f, 200.0f);
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
    const glm::mat4x4& view_mat() const
    {
        return m_view_mat;
    }
    glm::mat4x4& view_mat()
    {
        return m_view_mat;
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
    glm::mat4x4 m_view_mat;
};
} // namespace rendersystem