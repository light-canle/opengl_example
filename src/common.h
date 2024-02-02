#ifndef __COMMON_H__
#define __COMMON_H__

#include <memory>
#include <string>
#include <optional>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>; \
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;

std::optional<std::string> LoadTextFile(const std::string& filename);

// 거리에 따라 세 감쇠 상수를 근사해주는 함수
// source : https://www.youtube.com/watch?v=JAW3MGlfDwY&list=PLvNHCGtd4kh_cYLKMP_E-jwF3YKpDP4hf&index=23
glm::vec3 GetAttenuationCoeff(float distance);

#endif // __COMMON_H__