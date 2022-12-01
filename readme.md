## Examples

### Read JSON from a file

To create a json object by reading a JSON file:

You can use c++ file stream: 

```cpp
#include "Json.h"
#include <fstream>
using namespace wfrest;

std::ifstream f("example.json");
Json data = Json::parse(f); 
```

Or you can use `FILE*`

```cpp
FILE* fp = fopen("example.json", "r");
Json data = Json::parse(fp);
fclose(fp);
```

### Creating json objects from string 

Using (raw) string literals and json::parse

```cpp
Json data = Json::parse(R"(
{
  "pi": 3.141,
  "happy": true
}
)");
```

## Creating json objects by initializer list

```cpp
Json data = Json::Object{
    {"null", nullptr},
    {"integer", 1},
    {"float", 1.3},
    {"boolean", true},
    {"string", "something"},
    {"array", Json::Array{1, 2}},
    {"object",
     Json::Object{
         {"key", "value"},
         {"key2", "value2"},
     }},
};
```

## Create simple json value

```cpp
Json null1 = nullptr;

Json num1 = 1;

Json num2 = 1.0;

Json bool1 = true;

Json bool2 = false;

Json str1 = "string";

Json obj1 = Json::Object();

Json arr1 = Json::Array();
```

## Build Json::Object and Json::Array

### use operator[]

The JSON values can be constructed (comfortably) by using standard index operators:

Use operator[] to assign values to JSON objects:

```cpp
// create an empty structure (null)
Json data;
std::cout << "empty structure is " << data << std::endl;

// add a number that is stored as double (note the implicit conversion of j
// to an object)
data["pi"] = 3.141;

// add a Boolean that is stored as bool
data["happy"] = true;

// add a string that is stored as std::string
data["name"] = "chanchan";

// add another null object by passing nullptr
data["nothing"] = nullptr;

// add an object inside the object
data["answer"]["everything"] = 42;
```

### use push_back

Object case:

```cpp
Json data;

data.push_back("pi", 3.141);
data.push_back("happy", true);
data.push_back("name", "chanchan");
data.push_back("nothing", nullptr);
Json answer;
answer.push_back("everything", 42);
data.push_back("answer", answer);
```

Array Case:

```cpp
Json data;
data.push_back(3.141);
data.push_back(true);
data.push_back("chanchan");
data.push_back(nullptr);
Json arr;
arr.push_back(42);
arr.push_back("answer");
data.push_back(arr);
```

### Explict declare Json type 

If you want to be explicit or express the json type is array or object, the functions Json::Array and Json::Object will help:

```cpp
Json empty_object_explicit = Json::Object();
Json empty_array_explicit = Json::Array();
```

## Parse and dump

```cpp
Json js = Json::parse(R"({"test1":false})");
```

```cpp 
Json data;
data["key"]["chanchan"] = 1;

// default compact json string
{"key":{"chanchan":1}}

// using member function
std::string dump_str = data.dump();

// streams
std::ostringstream os;
os << data;
std::cout << os.str() << std::endl;

// For pretty stringification, there is the option to choose the identation size in number of spaces:
{
  "key": {
    "chanchan": 1
  }
}
std::string dump_str_pretty = data.dump(2);
```

