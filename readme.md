## Examples

### Read JSON from a file

To create a json object by reading a JSON file:

```cpp
#include "Json.h"
#include <fstream>
using namespace wfrest;

std::ifstream f("example.json");
Json data = Json::parse(f); 
```

### Creating json objects from string 

// Using (raw) string literals and json::parse

```cpp
Json data = Json::parse(R"(
{
  "pi": 3.141,
  "happy": true
}
)");
```

## Build Json::Object and Json::Array

### use operator[]

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
Json::Object obj
Json::Array arr;
```


