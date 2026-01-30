#pragma once

#include <format>
#include <string>

#include "strutil.hpp"

struct Minecraft_identifiable {
  std::string mc_namespace;
  std::string object_name;

  Minecraft_identifiable(void) = delete;

  explicit Minecraft_identifiable(std::string namespace_,
                                  std::string object_name)
      : mc_namespace{namespace_}, object_name{object_name} {}

  std::string id(void) const {
    const auto id_format = std::format("{}:{}", mc_namespace, object_name);
    return nospaces(tolower(id_format));
  }

  std::string item_id(void) const {
    const auto id_format = std::format("{}:item/{}", mc_namespace, object_name);
    return nospaces(tolower(id_format));
  }
};
