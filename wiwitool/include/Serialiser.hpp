// A serialiser/deserialiser that exports a file to genpath/import.json

#include <filesystem>
#include <string>

class Serialiser {
public:
  static void serialise(std::string);
  static std::string deserialise(void);

  static inline auto import_filename = std::filesystem::path{"import.json"};
};
