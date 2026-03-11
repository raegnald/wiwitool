// A Minecraft pack generator for custom music discs

#pragma once

#include "Minecraft_pack.hpp"
#include "music/Music_disc.hpp"

static std::string default_music_discs_namespace{"custommusicdiscs"};

class Music_discs_pack : public Minecraft_pack {
public:
  Music_discs_pack(void);
  virtual ~Music_discs_pack(void) = default;

  void set_discs(std::vector<Music_disc> discs);

protected:
  virtual void generate_data(void) override;
  virtual void generate_resource(void) override;

private:
  std::string music_discs_namespace{default_music_discs_namespace};
  std::vector<Music_disc> discs{};

  // std::vector<std::string> get_disc_ids(void) const;
};
