#pragma once

#include <string>
#include <vector>

#include "Minecraft_pack.hpp"

#include "Paintings_pack/Painting.hpp"

static std::string default_paintings_namespace{"custompaintings"};

class Paintings_pack : public Minecraft_pack {
private:
  std::string paintings_namespace{default_paintings_namespace};
  std::vector<Painting> paintings{};

  std::vector<std::string> get_painting_ids(void) const;

  void generate_painting_json(std::filesystem::path directory);

protected:
  virtual void generate_data(void) override;
  virtual void generate_resource(void) override;

public:
  Paintings_pack(void);

  virtual ~Paintings_pack() = default;

  Painting &add_painting(std::string filename);
};
