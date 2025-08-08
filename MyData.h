#ifndef MYDATA_H
#define MYDATA_H

#include "ValidatedJson.h"

class MyData : public ValidatedJson
{
public:
  MyData(JsonData&& data) :
    ValidatedJson(std::move(data))
  {
    Optional("name", _name, "No name provided");
    Required("description", _description);
  }

  std::string ToString() const
  {
    std::stringstream ss;
    ss << "MyData: name = " << _name << ", description = " << _description << std::endl;
    return ss.str();
  }

private:
  std::string _name;
  std::string _description;
};

#endif // MYDATA_H
