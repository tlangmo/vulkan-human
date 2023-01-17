#pragma once
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

namespace rendersystem
{
class Component
{
};

inline constexpr uint32_t id_from_name(const char* str, size_t n, uint32_t basis = UINT32_C(2166136261))
{
    return n == 0 ? basis : id_from_name(str + 1, n - 1, (basis ^ str[0]) * UINT32_C(16777619));
}
#define COMPONENT_ID(NAME) id_from_name(#NAME, strlen(#NAME))
#define DEFINE_COMPONENT_ID(NAME)                                                                                      \
    constexpr static const uint32_t id()                                                                               \
    {                                                                                                                  \
        return COMPONENT_ID(NAME);                                                                                     \
    }

class Entity
{
  public:
    template <typename T> void add_component(std::shared_ptr<T> cmp)
    {
        m_components[T::id()] = cmp;
    }
    template <typename T> std::shared_ptr<T> get_component() const
    {
        auto it = m_components.find(T::id());
        if (it != m_components.end())
        {
            return std::static_pointer_cast<T>(it->second);
        }
        else
        {
            return std::shared_ptr<T>();
        }
    }

  private:
    std::unordered_map<uint32_t, std::shared_ptr<Component>> m_components;
};
} // namespace rendersystem