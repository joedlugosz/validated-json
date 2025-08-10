#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "../ValidatedJson.h"

// File tests /////////////////////////////////////////////////////////////////

TEST_CASE("JSON file is loaded successfully", "[file]") {
  JsonFile jsonFile("test.json");
  REQUIRE(jsonFile.GetRoot().isObject());
}

TEST_CASE("JsonFile constructor throws if file is not found", "[file]") {
  try {
    JsonFile file("not_found.json");
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "Could not open JSON file: not_found.json");
  }
}

// JSON tests /////////////////////////////////////////////////////////////////

TEST_CASE("ValidatedJson constructor throws if JSON is empty", "[json]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) : ValidatedJson(data) {}
  };

  try {
    TestValidatedJson json{JsonString{""}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == 
    "JSON parsing error: * Line 1, Column 1\n"
    "  Syntax error: value, object or array expected.\n");
  }
}

// Presence tests ///////////////////////////////////////////////////////////

TEST_CASE("ValidatedJson constructor throws if required JSON value is missing", "[presence]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) : 
      ValidatedJson(data),
      testInt(Required<int>("testInt"))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "Required key \"testInt\" not found");
  }
}

TEST_CASE("ValidatedJson constructor does not throw if required JSON value is present", "[presence]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) : 
      ValidatedJson(data),
      testInt(Required<int>("testInt"))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 42}"}};
  } catch (const std::runtime_error& e) {
    FAIL("Exception was thrown");
  }
}

TEST_CASE("ValidatedJson constructor does not throw if optional JSON value is missing", "[presence]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) : 
      ValidatedJson(data),
      testInt(Optional<int>("testInt", 0))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 42}"}};
  } catch (const std::runtime_error& e) {
    FAIL("Exception was thrown");
  }
}

TEST_CASE("ValidatedJson constructor does not throw if optional JSON value is present", "[presence]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) : 
      ValidatedJson(data),
      testInt(Optional<int>("testInt", 0))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 42}"}};
  } catch (const std::runtime_error& e) {
    FAIL("Exception was thrown");
  }
}

// Parsing tests ///////////////////////////////////////////////////////////

TEST_CASE("Integer value is correctly parsed", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) : 
      ValidatedJson(data),
      testInt(Required<int>("testInt"))
    {}
    int testInt;
  };
  TestValidatedJson json{JsonString{"{\"testInt\": 10}"}};
  REQUIRE(json.testInt == 10);
}

TEST_CASE("String value is correctly parsed", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testString(Required<std::string>("testString"))
    {}
    std::string testString;
  };
  TestValidatedJson json{JsonString{"{\"testString\": \"Hello\"}"}};
  REQUIRE(json.testString == "Hello");
}

TEST_CASE("Boolean value is correctly parsed", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testBool(Required<bool>("testBool"))
    {}
    bool testBool;
  };
  TestValidatedJson json{JsonString{"{\"testBool\": true}"}};
  REQUIRE(json.testBool == true);
}

TEST_CASE("Double value is correctly parsed", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testDouble(Required<double>("testDouble"))
    {}
    double testDouble;
  };
  TestValidatedJson json{JsonString{"{\"testDouble\": 3.14}"}};
  REQUIRE(json.testDouble == Catch::Approx(3.14));
}

TEST_CASE("Nested JSON object is correctly parsed", "[parsing]") {
  class NestedData : public ValidatedJson {
  public:
    // NestedData() : ValidatedJson() {}
    NestedData(const JsonData& data) :
      ValidatedJson(data),
      age(Required<int>("age"))
    {}
    int age;
  };

  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      nested(Required<NestedData>("nested"))
    {}
    NestedData nested;
  };

  TestValidatedJson json{JsonString{"{\"nested\": {\"age\": 30}}"}};
  REQUIRE(json.nested.age == 30);
}

TEST_CASE("JSON array is correctly parsed", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      values(Required<std::vector<int>>("values"))
    {}
    std::vector<int> values;
  };

  TestValidatedJson json{JsonString{"{\"values\": [1, 2, 3]}"}};
  REQUIRE(json.values.size() == 3);
  REQUIRE(json.values[0] == 1);
  REQUIRE(json.values[1] == 2);
  REQUIRE(json.values[2] == 3);
}

TEST_CASE("ValidatedJson constructor throws if JSON value is not a string", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testString(Required<std::string>("testString"))
    {}
  private:
    std::string testString;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testString\": 123}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, expected a string value for key \"testString\"");
  }
}

TEST_CASE("ValidatedJson constructor throws if JSON value is not an integer", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt"))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": \"not an int\"}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, expected an integer value for key \"testInt\"");
  }
}

TEST_CASE("ValidatedJson constructor throws if JSON value is not a double", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testDouble(Required<double>("testDouble"))
    {}
  private:
    double testDouble;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testDouble\": \"not a double\"}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, expected a double value for key \"testDouble\"");
  }
}

TEST_CASE("ValidatedJson constructor throws if JSON value is not a boolean", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testBool(Required<bool>("testBool"))
    {}
  private:
    bool testBool;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testBool\": \"not a bool\"}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, expected a boolean value for key \"testBool\"");
  }
}

TEST_CASE("ValidatedJson constructor throws if JSON value is not an object", "[parsing]") {
  class NestedData : public ValidatedJson {
  public:
    // NestedData() : ValidatedJson() {}
    NestedData(const JsonData& data) : ValidatedJson(data) {}
  };

  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      nested(Required<NestedData>("nested"))
    {}
  private:
    NestedData nested;
  };
  try {
    TestValidatedJson json{JsonString{"{\"nested\": \"not an object\"}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, expected a JSON object for key \"nested\"");
  }
}

TEST_CASE("ValidatedJson constructor throws if JSON value is not an array", "[parsing]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      values(Required<std::vector<int>>("values"))
    {}
  private:
    std::vector<int> values;
  };
  try {
    TestValidatedJson json{JsonString{"{\"values\": \"not an array\"}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, expected a JSON array for key \"values\"");
  }
}

// Validation tests ///////////////////////////////////////////////////////////

TEST_CASE("ValidatedJson constructor throws if int JSON value is below minimum", "[validation]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").AboveMin(10))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 9}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, value for key \"testInt\" is below minimum of 10");
  }
}

TEST_CASE("ValidatedJson constructor does not throw if int JSON value is above minimum", "[validation]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").AboveMin(10))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 11}"}};
  } catch (const std::runtime_error& e) {
    FAIL("Exception was thrown");
  }
}

TEST_CASE("ValidatedJson constructor throws if int JSON value is above maximum", "[validation]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").BelowMax(10))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 11}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, value for key \"testInt\" is above maximum of 10");
  }
}

TEST_CASE("ValidatedJson constructor does not throw if int JSON value is below maximum", "[validation]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").BelowMax(10))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 9}"}};
  } catch (const std::runtime_error& e) {
    FAIL("Exception was thrown");
  }
}

TEST_CASE("ValidatedJson constructor does not throw if int JSON value is within range", "[validation]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").WithinRange(10, 20))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 11}"}};
  } catch (const std::runtime_error& e) {
    FAIL("Exception was thrown");
  }
}

TEST_CASE("ValidatedJson constructor throws if int JSON value is below range", "[validation]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").WithinRange(10, 20))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 9}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, value for key \"testInt\" is outside range 10 to 20");
  }
}

TEST_CASE("ValidatedJson constructor throws if int JSON value is above range", "[validation]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").WithinRange(10, 20))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 21}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, value for key \"testInt\" is outside range 10 to 20");
  }
}

TEST_CASE("ValidatedJson constructor does not throw if value is in permitted list", "[validation]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testValue(Required<int>("testValue").MemberOf({1, 2, 3}))
    {}
  private:
    int testValue;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testValue\": 2}"}};
  } catch (const std::runtime_error& e) {
    FAIL("Exception was thrown");
  }
}

TEST_CASE("ValidatedJson constructor throws if value is not in permitted list", "[validation]") {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testValue(Required<int>("testValue").MemberOf({1, 2, 3}))
    {}
  private:
    int testValue;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testValue\": 4}"}};
    FAIL("Expected exception not thrown");
  } catch (const std::runtime_error& e) {
    REQUIRE(std::string(e.what()) == "In JSON data, value for key \"testValue\" must be one of: 1 2 3");
  }
}
