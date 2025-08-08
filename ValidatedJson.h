#ifndef VALIDATED_JSON_H
#define VALIDATED_JSON_H

#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <json/json.h>
#include <iostream>
#include <type_traits>

class JsonData
{
public:
  explicit JsonData(std::istream&& stream)
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

  explicit JsonData(const Json::Value root) :
    _root(root)
  {}

  Json::Value GetRoot() const { return _root; }

protected:
  Json::Value _root;

private:
  std::string _errors;
};

class JsonFile : public JsonData
{
public:
  explicit JsonFile(const std::string& path) :
    JsonData(Open(path))
  {}

private:
  static std::ifstream Open(const std::string& path)
  {
    std::ifstream ifs;
    ifs.open(path);
    if (!ifs.is_open())
    {
      throw std::runtime_error("Could not open JSON file: " + path);
    }
    return ifs;
  }
};

class JsonString : public JsonData
{
public:
  explicit JsonString(const std::string& string) :
    JsonData(std::stringstream(string))
  {}
};

class ValidatedJson
{
public:
  explicit ValidatedJson(const JsonData& data) :
    _root(data.GetRoot())
  {}

  ValidatedJson() = delete;
  ValidatedJson(const ValidatedJson&) = default;
  ValidatedJson(ValidatedJson&&) = default;
  ValidatedJson& operator=(const ValidatedJson&) = default;
  ValidatedJson& operator=(ValidatedJson&&) = default;
  ~ValidatedJson() = default;

  Json::Value GetRoot() const { return _root; }

  template<typename T>
  T Required(const std::string& key) const
  {
    if (!_root.isMember(key))
    {
        throw std::runtime_error("Required key \"" + key + "\" not found");
    }

    return ParseValue<T>(key);
  }

  template<typename T>
  T Optional(const std::string& key, T defaultValue) const
  {
    if (!_root.isMember(key))
    {
      return defaultValue;
    }

    return ParseValue<T>(key);
  }

  template<typename T>
  T ParseValue(const std::string& key) const
  {
    if constexpr (std::is_same_v<T, std::string>) {
      return _root[key].asString();
    } else if constexpr (std::is_same_v<T, int>) {
      return _root[key].asInt();
    } else if constexpr (std::is_same_v<T, double>) {
      return _root[key].asDouble();
    } else if constexpr (std::is_same_v<T, bool>) {
      return _root[key].asBool();
    } else {
      static_assert(false && sizeof(T), "Unsupported type for ParseValue()");
    }
  }

protected:
  Json::Value _root;
};

#endif // VALIDATED_JSON_H
