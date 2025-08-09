
#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <json/json.h>
#include <iostream>
#include <type_traits>

#include "ValidatedJson.h"

JsonData::JsonData(std::istream&& stream)
{
  if (!stream.good())
  {
    throw std::runtime_error("Invalid input stream for JSON data.");
  }

  Json::parseFromStream(Json::CharReaderBuilder(), stream, &_root, &_errors);

  if (!_errors.empty())
  {
    throw std::runtime_error("JSON parsing error: " + _errors);
  }
}

JsonData::JsonData(const Json::Value root) :
  _root(root)
{}

JsonFile::JsonFile(const std::string& path) :
  JsonData(Open(path))
{}

std::ifstream JsonFile::Open(const std::string& path)
{
  std::ifstream ifs;
  ifs.open(path);
  if (!ifs.is_open())
  {
    throw std::runtime_error("Could not open JSON file: " + path);
  }
  return ifs;
}

JsonString::JsonString(const std::string& string) :
  JsonData(std::stringstream(string))
{}

ValidatedJson::ValidatedJson(const JsonData& data) :
  _root(data.GetRoot())
{}

// Special case for default value supplied to strings
void ValidatedJson::Optional(const std::string& key, std::string& value, const char* defaultValue) const {
  Optional(key, value, std::string(defaultValue));
}
