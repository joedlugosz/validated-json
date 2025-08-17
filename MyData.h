#ifndef MYDATA_H
#define MYDATA_H

#include "ValidatedJson.h"

class MyData2 : public ValidatedJson
{
public:
  MyData2(JsonData&& data) :
    ValidatedJson(std::move(data)),
    _age(Required<int>("age").Max(18))
  {}

  std::string ToString() const
  {
    std::stringstream ss;
    ss << "MyData2: age = " << _age << std::endl;
    return ss.str();
  }
  
private:
  int _age;
};

class MyData : public ValidatedJson
{
public:
  MyData(JsonData&& data) :
    ValidatedJson(std::move(data)),
    _name       (Optional<std::string>      ("name", "No name provided")),
    _description(Required<std::string>      ("description")),
    _nestedData (Required<MyData2>          ("nested")),
    _values     (Required<std::vector<int>> ("values"))
  {}

  std::string ToString() const
  {
    std::stringstream ss;
    ss << "MyData: name = " << _name
       << ", description = " << _description
       << ", nested = " << _nestedData.ToString()
       << ", values = [";
    for (const auto& value : _values) {
      ss << value << " ";
    }
    ss << "]"
       << std::endl;
    return ss.str();
  }

private:
  std::string _name;
  std::string _description;
  std::vector<int> _values;
  MyData2 _nestedData;
};


struct PersonStruct
{
  int age;
  char name[20];
};

struct PeopleStruct
{
  PersonStruct people[10];
  int count;
};

class PersonValidator : public ValidatedJson
{
public:
  PersonValidator(JsonData&& json, PersonStruct &data) :
    ValidatedJson(std::move(json)),
    data(data)
  {
    Required("age", data.age).Min(0).Max(120);
    RequiredCString("name", data.name, sizeof(data.name));
  }
  PersonStruct &data;
};

class PeopleValidator : public ValidatedJson
{
public:
  PeopleValidator(JsonData&& json, PeopleStruct &data) :
    ValidatedJson(std::move(json)),
    data(data)
  {
    Required("count", data.count).Min(1).Max(10);
    RequiredCArray<PersonStruct, PersonValidator>("people", data.people, sizeof(data.people) / sizeof(data.people[0]));
  }
  PeopleStruct &data;
};

#endif // MYDATA_H
