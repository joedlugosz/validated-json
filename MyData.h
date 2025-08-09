#ifndef MYDATA_H
#define MYDATA_H

#include "ValidatedJson.h"

class MyData2 : public ValidatedJson
{
public:
  MyData2(JsonData&& data) :
    ValidatedJson(std::move(data))
  {
    Required("age", _age);
  }

  MyData2() {}

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
    ValidatedJson(std::move(data))
  {
    Optional("name", _name, "No name provided");
    Required("description", _description);
    Required("nested", _nestedData);
    Required("values", _values);
  }

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

#endif // MYDATA_H
