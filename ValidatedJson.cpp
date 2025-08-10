
#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <json/json.h>
#include <iostream>
#include <type_traits>

#include "ValidatedJson.h"

JsonData::JsonData(std::istream&& stream, std::string source) : 
  _source(source)
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

JsonData::JsonData(const Json::Value root, std::string source) : 
  _source(source),
  _root(root)
{}

JsonFile::JsonFile(const std::string& path) :
  JsonData(Open(path), "JSON file \"" + path + "\"")
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
  JsonData(std::stringstream(string), "JSON data")
{}

ValidatedJson::ValidatedJson(const JsonData& data) :
  _source(data.GetSource()),
  _root(data.GetRoot())
{}

ValidatedJsonField<std::string> ValidatedJson::Optional(const std::string& key, const char* defaultValue) const
{
  return Optional<std::string>(key, std::string(defaultValue));
}
