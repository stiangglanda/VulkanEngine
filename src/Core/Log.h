#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace Core
{

class Log
{
  public:
    static void Init();

    static std::shared_ptr<spdlog::logger> &GetCoreLogger()
    {
        return s_CoreLogger;
    }
    static std::shared_ptr<spdlog::logger> &GetClientLogger()
    {
        return s_ClientLogger;
    }

  private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

} // namespace Core

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::vec<L, T, Q> &vector)
{
    return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::mat<C, R, T, Q> &matrix)
{
    return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, glm::qua<T, Q> quaternion)
{
    return os << glm::to_string(quaternion);
}

// Core log macros
#define VE_CORE_TRACE(...) ::Core::Log::GetCoreLogger()->trace(__VA_ARGS__)

// TODO if the logger is nullptr we could initialize the logger
#define VE_CORE_INFO(...)                                                                                              \
    {                                                                                                                  \
        if (Core::Log::GetCoreLogger() != nullptr)                                                                     \
        {                                                                                                              \
            Core::Log::GetCoreLogger()->info(__VA_ARGS__);                                                             \
        }                                                                                                              \
    }

#define VE_CORE_WARN(...) ::Core::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VE_CORE_ERROR(...) ::Core::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VE_CORE_CRITICAL(...) ::Core::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define VE_TRACE(...) ::Core::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VE_INFO(...) ::Core::Log::GetClientLogger()->info(__VA_ARGS__)
#define VE_WARN(...) ::Core::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VE_ERROR(...) ::Core::Log::GetClientLogger()->error(__VA_ARGS__)
#define VE_CRITICAL(...) ::Core::Log::GetClientLogger()->critical(__VA_ARGS__)
