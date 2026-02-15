// Minecraft pack that enables the crafting recipe for invisible item
// frames.

#pragma once

#include "Minecraft_pack.hpp"
#include <string>

class Invisible_item_frame_pack : public Minecraft_pack {
public:
  Invisible_item_frame_pack(std::string mc_namespace = "invisible_item_frame");

protected:
  virtual void generate_data() override;
  virtual void generate_resource() override;

private:
  std::string item_frame_namespace_;
};
