#include <gtest/gtest.h>
#include "../ValidatedJson.h"

// File tests /////////////////////////////////////////////////////////////////

TEST(File, JSON_file_is_loaded_successfully) {
  JsonFile jsonFile("test.json");
  EXPECT_EQ(jsonFile.GetRoot().isObject(), true);
}

TEST(File, JsonFile_constructor_throws_if_file_is_not_found) {
  try {
    JsonFile file("not_found.json");
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "Could not open JSON file: not_found.json");
  }
}

// JSON tests /////////////////////////////////////////////////////////////////

TEST(JSON, ValidatedJson_constructor_throws_if_jsoncpp_has_errors) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) : ValidatedJson(data) {}
  };

  try {
    TestValidatedJson json{JsonString{""}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), 
    "JSON parsing error: * Line 1, Column 1\n"
    "  Syntax error: value, object or array expected.\n");
  }
}

// Presence tests ///////////////////////////////////////////////////////////

TEST(KeyPresence, ValidatedJson_constructor_throws_if_required_JSON_value_is_missing) {
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
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "Required key \"testInt\" not found in JSON data");
  }
}

TEST(KeyPresence, ValidatedJson_constructor_does_not_throw_if_required_JSON_value_is_present) {
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
    FAIL() << "Exception was thrown";
  }
}

TEST(KeyPresence, ValidatedJson_constructor_does_not_throw_if_optional_JSON_value_is_missing) {
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
    FAIL() << "Exception was thrown";
  }
}

TEST(KeyPresence, ValidatedJson_constructor_does_not_throw_if_optional_JSON_value_is_present) {
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
    FAIL() << "Exception was thrown";
  }
}

// Parsing tests ///////////////////////////////////////////////////////////

TEST(Parsing, Scalar_types_are_correctly_parsed) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testString(Required<std::string>("testString")),
      testInt(Required<int>("testInt")),
      testDouble(Required<double>("testDouble")),
      testBool(Required<bool>("testBool"))
    {}
    std::string testString;
    int testInt;
    double testDouble;
    bool testBool;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testString\": \"test\", \"testInt\": 10, \"testDouble\": 3.14, \"testBool\": true}"}};
    EXPECT_EQ(json.testString, "test");
    EXPECT_EQ(json.testInt, 10);
    EXPECT_NEAR(json.testDouble, 3.14, 0.001);
    EXPECT_EQ(json.testBool, true);
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Parsing, Inplace_scalar_types_are_correctly_parsed) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testString", testString);
      Required("testInt", testInt);
      Required("testDouble", testDouble);
      Required("testBool", testBool);
    }
    std::string testString;
    int testInt;
    double testDouble;
    bool testBool;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testString\": \"test\", \"testInt\": 10, \"testDouble\": 3.14, \"testBool\": true}"}};
    EXPECT_EQ(json.testString, "test");
    EXPECT_EQ(json.testInt, 10);
    EXPECT_NEAR(json.testDouble, 3.14, 0.001);
    EXPECT_EQ(json.testBool, true);
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Parsing, C_arrays_of_scalar_types_are_correctly_parsed) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      // Required("testString", testString);
      Required("testInt", testInt);
      Required("testDouble", testDouble);
      Required("testBool", testBool);
    }
    // std::string testString[4];
    int testInt[4];
    double testDouble[4];
    bool testBool[4];
  };
  try {
    TestValidatedJson json{JsonString{
      "{\"testString\": [\"test1\", \"test2\", \"test3\", \"test4\"],"
      "\"testInt\": [1, 2, 3, 4], "
      "\"testDouble\": [1.0, 2.0, 3.0, 4.0], "
      "\"testBool\": [true, false, true, false]}"}};
    // EXPECT_EQ(json.testString[0], "test1");
    // EXPECT_EQ(json.testString[1], "test2");
    // EXPECT_EQ(json.testString[2], "test3");
    // EXPECT_EQ(json.testString[3], "test4");
    EXPECT_EQ(json.testInt[0], 1);
    EXPECT_EQ(json.testInt[1], 2);
    EXPECT_EQ(json.testInt[2], 3);
    EXPECT_EQ(json.testInt[3], 4);
    EXPECT_NEAR(json.testDouble[0], 1.0, 0.001);
    EXPECT_NEAR(json.testDouble[1], 2.0, 0.001);
    EXPECT_NEAR(json.testDouble[2], 3.0, 0.001);
    EXPECT_NEAR(json.testDouble[3], 4.0, 0.001);
    EXPECT_EQ(json.testBool[0], true);
    EXPECT_EQ(json.testBool[1], false);
    EXPECT_EQ(json.testBool[2], true);
    EXPECT_EQ(json.testBool[3], false);
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Parsing, Nested_JSON_object_is_correctly_parsed) {
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
  EXPECT_EQ(json.nested.age, 30);
}

TEST(Parsing, Vector_is_correctly_parsed) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      values(Required<std::vector<int>>("values"))
    {}
    std::vector<int> values;
  };

  TestValidatedJson json{JsonString{"{\"values\": [1, 2, 3]}"}};
  EXPECT_EQ(json.values.size(), 3);
  EXPECT_EQ(json.values[0], 1);
  EXPECT_EQ(json.values[1], 2);
  EXPECT_EQ(json.values[2], 3);
}

// Type checking tests ///////////////////////////////////////////////////////////

TEST(TypeChecking, ValidatedJson_constructor_throws_if_JSON_value_is_not_a_string) {
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
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, expected a string value for key \"testString\"");
  }
}

TEST(TypeChecking, ValidatedJson_constructor_throws_if_JSON_value_is_not_an_integer) {
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
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, expected an integer value for key \"testInt\"");
  }
}

TEST(TypeChecking, ValidatedJson_constructor_throws_if_JSON_value_is_not_a_double) {
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
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, expected a double value for key \"testDouble\"");
  }
}

TEST(TypeChecking, ValidatedJson_constructor_throws_if_JSON_value_is_not_a_boolean) {
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
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, expected a boolean value for key \"testBool\"");
  }
}

TEST(TypeChecking, ValidatedJson_constructor_throws_if_JSON_value_is_not_an_object) {
  class NestedData : public ValidatedJson {
  public:
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
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, expected a JSON object for key \"nested\"");
  }
}

TEST(TypeChecking, ValidatedJson_constructor_throws_if_JSON_value_is_not_an_array) {
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
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, expected a JSON array for key \"values\"");
  }
}

// Validation tests ///////////////////////////////////////////////////////////

TEST(Validation, ValidatedJson_constructor_throws_if_int_JSON_value_is_below_minimum) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").Min(10))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 9}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testInt\" is below minimum of 10");
  }
}

TEST(Validation, ValidatedJson_constructor_does_not_throw_if_int_JSON_value_is_above_minimum) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").Min(10))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 11}"}};
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_constructor_throws_if_int_JSON_value_is_above_maximum) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").Max(10))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 11}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testInt\" is above maximum of 10");
  }
}

TEST(Validation, ValidatedJson_constructor_does_not_throw_if_int_JSON_value_is_below_maximum) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").Max(10))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 9}"}};
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_constructor_does_not_throw_if_int_JSON_value_is_within_range) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").Range(10, 20))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 11}"}};
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_constructor_throws_if_int_JSON_value_is_below_range) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").Range(10, 20))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 9}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testInt\" is outside range 10 to 20");
  }
}

TEST(Validation, ValidatedJson_constructor_throws_if_int_JSON_value_is_above_range) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      testInt(Required<int>("testInt").Range(10, 20))
    {}
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 21}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testInt\" is outside range 10 to 20");
  }
}

TEST(Validation, ValidatedJson_constructor_does_not_throw_if_value_is_in_permitted_list) {
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
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_constructor_throws_if_value_is_not_in_permitted_list) {
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
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testValue\" must be one of: 1 2 3");
  }
}

TEST(Validation, ValidatedJson_constructor_does_not_throw_if_file_value_is_found) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      file(Required<std::string>("file").File())
    {}
  private:
    std::string file;
  };
  try {
    TestValidatedJson json{JsonString{"{\"file\": \"test.json\"}"}};
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_constructor_throws_if_file_value_does_not_exist) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data),
      file(Required<std::string>("file").File())
    {}
  private:
    std::string file;
  };
  try {
    TestValidatedJson json{JsonString{"{\"file\": \"non_existent_file.txt\"}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, filename value for key \"file\" does not exist: non_existent_file.txt");
  }
}

// In-place validation tests ///////////////////////////////////////////////////////////

TEST(Validation, ValidatedJson_in_place_constructor_throws_if_int_JSON_value_is_below_minimum) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testInt", testInt).Min(10);
    }
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 9}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testInt\" is below minimum of 10");
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_does_not_throw_if_int_JSON_value_is_above_minimum) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testInt", testInt).Min(10);
    }
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 11}"}};
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_throws_if_int_JSON_value_is_above_maximum) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testInt", testInt).Max(10);
    }
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 11}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testInt\" is above maximum of 10");
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_does_not_throw_if_int_JSON_value_is_below_maximum) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testInt", testInt).Max(10);
    }
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 9}"}};
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_does_not_throw_if_int_JSON_value_is_within_range) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testInt", testInt).Range(10, 20);
    }
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 11}"}};
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_throws_if_int_JSON_value_is_below_range) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testInt", testInt).Range(10, 20);
    }
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 9}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testInt\" is outside range 10 to 20");
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_throws_if_int_JSON_value_is_above_range) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testInt", testInt).Range(10, 20);
    }
  private:
    int testInt;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testInt\": 21}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testInt\" is outside range 10 to 20");
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_does_not_throw_if_value_is_in_permitted_list) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testValue", testValue).MemberOf({1, 2, 3});    
    }
  private:
    int testValue;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testValue\": 2}"}};
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_throws_if_value_is_not_in_permitted_list) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("testValue", testValue).MemberOf({1, 2, 3});    
    }
  private:
    int testValue;
  };
  try {
    TestValidatedJson json{JsonString{"{\"testValue\": 4}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, value for key \"testValue\" must be one of: 1 2 3");
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_does_not_throw_if_file_value_is_found) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("file", file).File();
    }
  private:
    std::string file;
  };
  try {
    TestValidatedJson json{JsonString{"{\"file\": \"test.json\"}"}};
  } catch (const std::runtime_error& e) {
    FAIL() << "Exception was thrown";
  }
}

TEST(Validation, ValidatedJson_in_place_constructor_throws_if_file_value_does_not_exist) {
  class TestValidatedJson : public ValidatedJson {
  public:
    TestValidatedJson(const JsonData& data) :
      ValidatedJson(data)
    {
      Required("file", file).File();
    }
  private:
    std::string file;
  };
  try {
    TestValidatedJson json{JsonString{"{\"file\": \"non_existent_file.txt\"}"}};
    FAIL() << "Expected exception not thrown";
  } catch (const std::runtime_error& e) {
    EXPECT_EQ(std::string(e.what()), "In JSON data, filename value for key \"file\" does not exist: non_existent_file.txt");
  }
}
