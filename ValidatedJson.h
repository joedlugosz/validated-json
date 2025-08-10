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
#include <filesystem>

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
   * @param source A string describing the source of the JSON data.
   * @throws std::runtime_error if the stream is invalid or if parsing fails.
   */
  explicit JsonData(std::istream&& stream, std::string source = "JSON data");

  /**
   * @brief Constructor that takes existing parsed JSON data.
   * @param root JSON root value.
   * @param source A string describing the source of the JSON data.
   */
  explicit JsonData(const Json::Value root, std::string source = "JSON data");

  /**
   * @brief Get the Root value of the parsed JSON data.
   * @return Json::Value 
   */
  inline Json::Value GetRoot() const { return _root; }

  /**
   * @brief Get a string describing the source of the parsed JSON data.
   * @return const std::string&
   */
  inline const std::string& GetSource() const { return _source; }

protected:
  Json::Value _root;

private:
  std::string _errors;
  std::string _source;
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
 * @brief Class providing validation methods on a parsed JSON field.
 *        Functions return a ValidatedJsonField<T> object which can be used
 *        to chain validation methods. Can be cast to the original type.
 * @tparam T The type of the field being validated.
 */
template<typename T>
class ValidatedJsonField
{
public:
  ValidatedJsonField(const std::string& key, T value,
    const std::string &source) :
    _key(key), _value(std::move(value)), _source(source)
  {}

  /**
   * @brief Type conversion operator to allow direct use of the value.
   * @return The value of the field.
   */
  operator T() { return _value;}

  /**
   * @brief Validate a value against a minimum. Throw if not validated.
   *        Can only be used on types that support less-than comparison
   * @param min The minimum value
   */
  template<typename U = T>
    typename std::enable_if<
    std::is_convertible<decltype(std::declval<U>() < std::declval<U>()), bool>::value,
    ValidatedJsonField<T>
    >::type 
  AboveMin(const T& min) const
  {
    if (_value < min)
    {
      ThrowValidationError("is below minimum of " + std::to_string(min));
    }
    return *this;
  }

  /**
   * @brief Validate a value against a maximum. Throw if not validated.
   *        Can only be used on types that support greater-than comparison
   * @param value The value to validate
   * @param max The maximum value
   */
  template<typename U = T>
    typename std::enable_if<
    std::is_convertible<decltype(std::declval<U>() > std::declval<U>()), bool>::value,
    ValidatedJsonField<T>
    >::type 
  BelowMax(const T& max) const
  {
    if (_value > max)
    {
      ThrowValidationError("is above maximum of " + std::to_string(max));
    }
    return *this;
  }

  /**
   * @brief Validate a value against a min-max range. Throw if not validated.
   *        Can only be used on types that support less/greater-than comparison
   * @param min The minimum value
   * @param max The maximum value
   */
  template<typename U = T>
    typename std::enable_if<
    std::is_convertible<decltype(std::declval<U>() > std::declval<U>()), bool>::value &&
    std::is_convertible<decltype(std::declval<U>() < std::declval<U>()), bool>::value,
    ValidatedJsonField<T>
    >::type 
  WithinRange(const T& min, const T& max) const
  {
    if (_value < min || _value > max)
    {
      ThrowValidationError("is outside range " + 
        std::to_string(min) + " to " + std::to_string(max));
    }
    return *this;
  }

  /**
   * @brief Validate a value against a set of permitted values. Throw if not
   *        validated.
   * @param permitted The permitted values
   */
  ValidatedJsonField<T> MemberOf(const std::initializer_list<T> permitted) const
  {
    for (const auto& p : permitted) {
      if (_value == p) return *this;
    }

    std::stringstream ss;
    ss << "must be one of:";
    for (const auto& p : permitted) {
      ss << " " << std::to_string(p);
    }
    ThrowValidationError(ss.str());
    return *this; // For compilation, this line will never be reached.
  }

  /**
   * @brief Validate a value as a filename. Throw if the file does not exist.
   * @param prefix An optional path to prepend to the filename.
   */
  template<typename U = T>
    typename std::enable_if<std::is_same<U, std::string>::value, ValidatedJsonField<T>>::type
  File(const std::filesystem::path prefix = "") const
  {
    std::filesystem::path path = prefix / _value;
    if (!std::filesystem::exists(path)) {
      throw std::runtime_error("In " + _source + ", filename value for key \""
        + _key + "\" does not exist: " + path.string());
    }
    return *this; // For compilation, this line will never be reached.
  }

private:
  inline void ThrowValidationError(const std::string& message) const
  {
    throw std::runtime_error("In " + _source + ", value for key \"" + _key + "\" " + message);
  }

  const std::string& _key;
  const T _value;
  const std::string &_source;
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

  /**
   * @brief Get a string describing the source of the parsed JSON data.
   * @return const std::string&
   */
  inline const std::string& GetSource() const { return _source; }

protected:
  /**
   * @brief Construct a new Validated Json object from JsonData.
   *        Called from derived classes.
   * @param data JsonData object containing the parsed JSON data.
   */
  explicit ValidatedJson(const JsonData& data);

  ValidatedJson() = delete;
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
  ValidatedJsonField<T> Required(const std::string& key) const
  {
    if (!_root.isMember(key))
    {
        throw std::runtime_error("Required key \"" + key + "\" not found");
    }

    return ValidatedJsonField<T>(key, ParseValue<T>(key, _root[key]), _source);
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
  ValidatedJsonField<T> Optional(const std::string& key, const T& defaultValue) const
  {
    if (!_root.isMember(key))
    {
      return ValidatedJsonField<T>(key, defaultValue, _source);
    }
    else
    {
      return ValidatedJsonField<T>(key, ParseValue<T>(key, _root[key]), _source);
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
  ValidatedJsonField<std::string> Optional(const std::string& key, const char* defaultValue) const;

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
        ThrowParsingError(key, "a string value");
      }
      return value.asString();
    } else if constexpr (std::is_same_v<T, int>) {
      if (!value.isInt()) {
        ThrowParsingError(key, "an integer value");
      }
      return value.asInt();
    } else if constexpr (std::is_same_v<T, double>) {
      if (!value.isDouble()) {
        ThrowParsingError(key, "a double value");
      }
      return value.asDouble();
    } else if constexpr (std::is_same_v<T, bool>) {
      if (!value.isBool()) {
        ThrowParsingError(key, "a boolean value");
      }
      return value.asBool();
    } else if constexpr (std::is_base_of_v<ValidatedJson, T>) {
      // Deal with nested JSON objects
      if (!value.isObject()) {
        ThrowParsingError(key, "a JSON object");
      }
      return T(JsonData(value));
    } else if constexpr (is_vector<T>::value) {
      std::cout << "vector" << std::endl;
      // Deal with JSON arrays
      if (!value.isArray()) {
        ThrowParsingError(key, "a JSON array");
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

  inline void ThrowParsingError(const std::string &key, const std::string& description) const
  {
    throw std::runtime_error("In " + _source + ", expected " + description + " for key \"" + key + "\"");
  }

protected:
  Json::Value _root;
  const std::string &_source;
};

#endif // VALIDATED_JSON_H
