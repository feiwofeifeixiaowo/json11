#include <iostream>
#include <string>
#include "json_src/json.h"

using namespace std;
using namespace yjson;
int main() {
    std::cout << "Hello, World!" << std::endl;

    //test number
    string jsonStr = "0.0";
    json json1;
    json1.parse(json1.value, jsonStr);
    cout << json1.value.type << endl;
    cout << json1.value.number << endl;
    return 0;
}