// Minecraft pack that enables the crafting recipe for invisible item
// frames.

#pragma once

#include "Minecraft_pack.hpp"
#include <format>
#include <string>

class Invisible_item_frame_pack : public Minecraft_pack {
public:
  Invisible_item_frame_pack(
      std::string mc_namespace = default_invisible_item_frame_namespace);

  void set_workspace_name(std::string workspace_name) {
    item_frame_namespace_ = std::format("{}_{}", workspace_name,
                                        default_invisible_item_frame_namespace);
  }

protected:
  virtual void generate_data() override;
  virtual void generate_resource() override;

private:
  static inline const auto default_invisible_item_frame_namespace =
      std::string{"invisible_item_frame"};

  std::string item_frame_namespace_;
};
