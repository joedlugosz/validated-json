#include "ValidatedJson.h"
#include "MyData.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <json_file_path>" << std::endl;
    return 1;
  }

  // try {
  //   auto json{JsonFile(argv[1])};
  //   std::cout << json.GetRoot() << std::endl;
  //   std::cout << "JSON file loaded successfully." << std::endl;
  // } catch (const std::exception& e) {
  //   std::cerr << "Error: " << e.what() << std::endl;
  //   return 1;
  // }

  // try {
  //   auto json{JsonString("{\"key\": \"value\"}")};
  //   std::cout << json.GetRoot() << std::endl; 
  //   std::cout << "JSON string loaded successfully." << std::endl;
  // } catch (const std::exception& e) {
  //   std::cerr << "Error: " << e.what() << std::endl;
  //   return 1;
  // }

  // try {
  //   auto data{MyData{JsonString("{\"description\": \"a test\"}")}};
  //   std::cout << data.ToString() << std::endl;
  //   std::cout << "JSON string loaded successfully." << std::endl;
  // } catch (const std::exception& e) { 
  //   std::cerr << "Error: " << e.what() << std::endl;
  //   return 1;
  // }

  try {
    auto data{MyData{JsonString("{\"description\": \"a test\", \"nested\": {\"age\": 30}"
      ", \"values\": [1, 2, 3]}")}};
    std::cout << data.ToString() << std::endl;
    std::cout << "JSON string loaded successfully." << std::endl;
  } catch (const std::exception& e) { 
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0; 
}
