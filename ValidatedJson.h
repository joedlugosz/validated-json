#ifndef VALIDATED_JSON_H
#define VALIDATED_JSON_H

#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <json/json.h>
#include <iostream>
#include <type_traits>
#include <vector>

/**
 * @brief Type trait to check if a type is a std::vector<T>.
 */
template<typename T>
struct is_vector : std::false_type {};
/**
 * @brief Type trait to check if a type is a std::vector<T>.
 */
template<typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};

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
   * @brief Helper function to open the file and throw an exception if it
   *        cannot be opened.
   * @param path Path to the JSON file.
   * @return std::ifstream 
   */
  static std::ifstream Open(const std::string& path);
};

/**
 * @brief Class to parse JSON data from a string.
 */
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

    value = ParseValue<T>(key, _root[key]);
  }

  /**
   * @brief Retrieve an optional key from the JSON data and provide a default
   *        value if the key is not found.
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
      value = ParseValue<T>(key, _root[key]);
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

  /**
   * @brief Validate a value against a minimum. Throw if not validated.
   *        Can only be used on types that support less-than comparison
   * @param key The JSON key for error message
   * @param value The value to validate
   * @param min The minimum value
   */
  template<typename T,
         typename = typename std::enable_if<
           std::is_convertible<decltype(std::declval<T>() < std::declval<T>()), bool>::value
         >::type>
  void AboveMin(const std::string& key, const T& value, const T& min) const
  {
    if (value < min)
    {
      throw std::runtime_error("Value for key \"" + key + "\" is below minimum: " + std::to_string(min));
    }
  }

  /**
   * @brief Validate a value against a maximum. Throw if not validated.
   *        Can only be used on types that support greater-than comparison
   * @param key The JSON key for error message
   * @param value The value to validate
   * @param max The maximum value
   */
  template<typename T,
         typename = typename std::enable_if<
           std::is_convertible<decltype(std::declval<T>() > std::declval<T>()), bool>::value
         >::type>
  void BelowMax(const std::string& key, const T& value, const T& max) const
  {
    if (value > max)
    {
      throw std::runtime_error("Value for key \"" + key + "\" is above maximum: " + std::to_string(max));
    }
  }

  /**
   * @brief Validate a value against a min-max range. Throw if not validated.
   *        Can only be used on types that support less/greater-than comparison
   * @param key The JSON key for error message
   * @param value The value to validate
   * @param min The minimum value
   * @param max The maximum value
   */
  template<typename T>
  inline void WithinRange(const std::string& key, const T& value, const T& min, const T& max) const
  {
    AboveMin(key, value, min);
    BelowMax(key, value, max);
  }

  /**
   * @brief Validate a value against a set of permitted values. Throw if not
   *        validated.
   * @param key The JSON key for error message
   * @param value The value to validate
   * @param permitted The permitted values
   */
  template<typename T>
  inline void MemberOf(const std::string& key, const T& value, const std::initializer_list<T> permitted) const
  {
    for (const auto& p : permitted) {
      if (value == p) return;
    }

    std::stringstream ss;
    ss << "Value for key " << key << " must be one of:";
    for (const auto& p : permitted) {
      ss << " " << std::to_string(p);
    }
    throw std::runtime_error(ss.str());
  }

private:
  /**
   * @brief Parse the value of a key from the JSON data.
   * @param key Key name to parse.
   * @return Parsed value of type T.
   */
  template<typename T>
  T ParseValue(const std::string &key, const Json::Value& value) const
  {
    // Add types here as necessary
    if constexpr (std::is_same_v<T, std::string>) {
      if (!value.isString()) {
        throw std::runtime_error("Expected a string value for key: " + key);
      }
      return value.asString();
    } else if constexpr (std::is_same_v<T, int>) {
      if (!value.isInt()) {
        throw std::runtime_error("Expected an integer value for key: " + key);
      }
      if (value.asInt() < Json::Value::minInt || value.asInt() > Json::Value::maxInt) {
        throw std::runtime_error("Integer value out of range for key: " + key);
      }
      return value.asInt();
    } else if constexpr (std::is_same_v<T, double>) {
      if (!value.isDouble()) {
        throw std::runtime_error("Expected a double value for key: " + key);
      }
      return value.asDouble();
    } else if constexpr (std::is_same_v<T, bool>) {
      if (!value.isBool()) {
        throw std::runtime_error("Expected a boolean value for key: " + key);
      }
      return value.asBool();
    } else if constexpr (std::is_base_of_v<ValidatedJson, T>) {
      // Deal with nested JSON objects
      if (!value.isObject()) {
        throw std::runtime_error("Expected a JSON object for key: " + key);
      }
      return T(JsonData(value));
    } else if constexpr (is_vector<T>::value) {
      std::cout << "vector" << std::endl;
      // Deal with JSON arrays
      if (!value.isArray()) {
        throw std::runtime_error("Expected a JSON array for key: " + key);
      }
      T result;
      for (const auto& element : value) {
        result.emplace_back(ParseValue<typename T::value_type>(key, element));
      }
      return result;
    } else {
      static_assert(false && sizeof(T), "Unsupported type for ParseValue()");
    }
  }

protected:
  Json::Value _root;
};

#endif // VALIDATED_JSON_H
