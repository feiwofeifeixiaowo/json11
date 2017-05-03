#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "json.h"


#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace xjson;
using namespace std;


string passJson(const string& filename);

int main() {
    // constructor
    unordered_map<string, int> hashTable = {
            {"a", 123},
            {"b", 456},
            {"c", 789}
    };

    // update
    hashTable["a"] = 1;
    // add new entry
    hashTable["d"] = 2;
    hashTable.insert({"e", 111});
    // Iterate and print keys and values of unordered_map
    for (const auto& n : hashTable )
        cout << "Key: " << n.first << "\tValue: " << n.second << endl;
    // Output values by key
    cout << "The value of d is :" << hashTable["d"] << endl;

    // init vector
    vector<int> states{1,2,3,4,5,6,7};
    states.push_back(8);

    // add vector into unordered_map
    unordered_map<string, vector<int>> ht;
    ht["state1"] = states;
    ht.insert({"c", vector<int>{1,1,1,1}});

    cout << "Values which key is 'c' in ht" << endl;
    for(auto& v : ht["c"])
        cout << v << "\t";
    cout << endl;

    /*
     * put array into unordered_map
     */
    int state1[3] = {0, 1, 2};
    int state2[2] = {3, 4};
    int state3[4] = {5, 6, 7, 8};

    // declare map to store int pointer value
    unordered_map<string, int*> u_map;

    // add into unordered_map
    u_map["a"] = state1;
    u_map["b"] = state2;
    u_map.insert({"c", state3});

    // update
    u_map["b"] = state3;

    // get pointer of array
    auto s1 = u_map["a"];
    int* s2 = u_map["b"];

    // accesses val in array
    cout << "Value of key a is: "<< s2[0] << endl;

    size_t size = sizeof(s1)/sizeof(s1[0]);
    for (int i = 0; i <= size ; ++i) {
        cout << "val " << i << " is: "<< s1[i] << endl;
    }

    /******************************************************/
    string ret = passJson("../test/json_file/pass1.json");
    cout << ret << endl;

    return 0;
}

string getJsonStr(const string& filename) {
    ifstream in(filename);
    if(!in.is_open()) throw runtime_error("cannot open file: " + string(filename));
    string jsonStr;
    while(in) {
        string line;
        getline(in, line);
        jsonStr += line + "\n";
    }
    return jsonStr;
}

string passJson(const string& filename) {
    string jsonStr = getJsonStr(filename);
    string errMsg;
    Json ret = Json::parse(jsonStr, errMsg);
    if(errMsg != "") {
        cerr << "ERROR expect pass, but fail" << endl;
        cerr << "file:" << filename << endl;
        cerr << jsonStr << endl;
        cerr << endl;
    }
    return ret.serialize();
}
