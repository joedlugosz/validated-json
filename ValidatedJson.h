#ifndef VALIDATED_JSON_H
#define VALIDATED_JSON_H

#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <json/json.h>
#include <iostream>
#include <type_traits>

/**
 *  @brief Class to parse JSON data which is provided to a ValidatedJson class.
 *  @see   ValidatedJson, JsonFile, JsonString
 */
 class JsonData
{
public:
  /**
   * @brief Constructor that reads JSON data from an input stream.
   * @param stream Input stream containing JSON data.
   * @throws std::runtime_error if the stream is invalid or if parsing fails.
   */
  explicit JsonData(std::istream&& stream);

  /**
   * @brief Constructor that takes existing parsed JSON data.
   * @param root JSON root value.
   */
  explicit JsonData(const Json::Value root);

  /**
   * @brief Get the Root value of the parsed JSON data.
   * @return Json::Value 
   */
  inline Json::Value GetRoot() const { return _root; }

protected:
  Json::Value _root;

private:
  std::string _errors;
};

/**
 * @brief Class to parse JSON data from a file.
 * @see   JsonData
 */
class JsonFile : public JsonData
{
public:
  /**
   * @brief Constructor that reads JSON data from a file.
   * @param path Path to the JSON file.
   * @throws std::runtime_error if the file cannot be opened.
   */
  explicit JsonFile(const std::string& path);

private:
  /**
   * @brief Helper function to open the file and throw an error if it cannot be opened.
   * @param path Path to the JSON file.
   * @return std::ifstream 
   */
  static std::ifstream Open(const std::string& path);
};

// Class to parse JSON data from a string.
class JsonString : public JsonData
{
public:
  /**
   * @brief Constructor that reads JSON data from a string.
   * @param path The JSON string.
   */
  explicit JsonString(const std::string& string);
};

/**
 * @brief Class to perform validation on parsed JSON data.
 *        Derive from this class to implement specific validation logic.
 * @see   JsonData
 */
class ValidatedJson
{
public:
  /**
   * @brief Get the Root object
   * @return Json::Value 
   */
  inline Json::Value GetRoot() const { return _root; }

protected:
  /**
   * @brief Construct a new Validated Json object from JsonData.
   *        Called from derived classes.
   * @param data JsonData object containing the parsed JSON data.
   */
  explicit ValidatedJson(const JsonData& data);

  ValidatedJson() = default;
  ValidatedJson(const ValidatedJson&) = default;
  ValidatedJson(ValidatedJson&&) = default;
  ValidatedJson& operator=(const ValidatedJson&) = default;
  ValidatedJson& operator=(ValidatedJson&&) = default;
  ~ValidatedJson() = default;

  /**
   * @brief Retrieve a required key from the JSON data.
   * @param key Key name to retrieve.
   * @param value Reference to store the retrieved value.
   * @throws std::runtime_error if the key is not found in the JSON data.
   * @return None 
   */
  template<typename T>
  void Required(const std::string& key, T& value) const
  {
    if (!_root.isMember(key))
    {
        throw std::runtime_error("Required key \"" + key + "\" not found");
    }

    value = ParseValue<T>(key);
  }

  /**
   * @brief Retrieve an optional key from the JSON data.
   * @param key Key name to retrieve.
   * @param value Reference to store the retrieved value.
   * @param defaultValue Default value to use if the key is not found.
   * @return None 
   */
  template<typename T>
  void Optional(const std::string& key, T& value, const T& defaultValue) const
  {
    if (!_root.isMember(key))
    {
      value = defaultValue;
    }
    else
    {
      value = ParseValue<T>(key);
    }
  }

  /**
   * @brief Retrieve an optional key from the JSON data.
   *        This is a special case for strings where a default value is
   *        provided as a C-style string.
   * @param key Key name to retrieve.
   * @param value Reference to store the retrieved value.
   * @param defaultValue Default value to use if the key is not found.
   * @return None 
   */
  void Optional(const std::string& key, std::string& value, const char* defaultValue) const;

private:
  /**
   * @brief Parse the value of a key from the JSON data.
   * @param key Key name to parse.
   * @return Parsed value of type T.
   */
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
    } else if constexpr (std::is_base_of_v<ValidatedJson, T>) {
      // Pass the nested JSON object into the constructor of the derived class
      return T(JsonData(_root[key]));
     } else {
      static_assert(false && sizeof(T), "Unsupported type for ParseValue()");
    }
  }

protected:
  Json::Value _root;
};

#endif // VALIDATED_JSON_H
