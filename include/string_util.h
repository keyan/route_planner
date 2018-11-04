#include <string>

// Escape any special characters to avoid issues with clientside string usage.
inline std::string escape_json(const std::string& input) {
  std::string output;
  // +4 assumes two backslashes on avg
  output.reserve(input.size() + 4);
  for (const char letter : input) {
    switch (letter) {
    case '\\':
      output += "\\\\";
      break;
    case '"':
      output += "\\\"";
      break;
    case '/':
      output += "\\/";
      break;
    case '\b':
      output += "\\b";
      break;
    case '\f':
      output += "\\f";
      break;
    case '\n':
      output += "\\n";
      break;
    case '\r':
      output += "\\r";
      break;
    case '\t':
      output += "\\t";
      break;
    default:
      output.append(1, letter);
      break;
    }
  }
  return output;
}
