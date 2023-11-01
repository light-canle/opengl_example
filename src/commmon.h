#ifndef __COMMON_H__
#define __COMMON_H__

#include <memory>
#include <string>
#include <optional>

std::optional<std::string> LoadTextFile(const std::string& filename);

#endif // __COMMON_H__