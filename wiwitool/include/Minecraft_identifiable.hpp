#pragma once

#include <format>
#include <string>

#include "util/strutil.hpp"

class Minecraft_identifiable {
public:
  Minecraft_identifiable(void) = delete;

  explicit Minecraft_identifiable(std::string namespace_,
                                  std::string object_name)
      : mc_namespace{namespace_}, object_name{object_name} {}

  // Create a Minecraft identifier ad hoc given a namespace and an
  // object name.
  static std::string adhoc(std::string adhoc_namespace, std::string adhoc_name) {
    const auto id_format = std::format("{}:{}", adhoc_namespace, adhoc_name);
    return nospaces(tolower(id_format));
  }

  // Create a Minecraft identifier for an inventory item ad hoc given
  // a namespace and an object name.
  static std::string adhoc_item(std::string adhoc_namespace, std::string adhoc_name) {
    const auto id_format = std::format("{}:item/{}", adhoc_namespace, adhoc_name);
    return nospaces(tolower(id_format));
  }

  std::string id(void) const { return adhoc(mc_namespace, object_name); }
  std::string item_id(void) const {return adhoc_item(mc_namespace, object_name); }

private:
  std::string mc_namespace;
  std::string object_name;
};
