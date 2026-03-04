// A serialiser/deserialiser that exports a file to genpath/import.json

#include <filesystem>
#include <vector>

class Serialiser {
public:
  static void serialise(std::vector<uint8_t>);
  static std::vector<uint8_t> deserialise(void);

  static inline auto import_filename = std::filesystem::path{"wiwitool.data"};
};
