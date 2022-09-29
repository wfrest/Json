#ifndef WFREST_JSON_H_
#define WFREST_JSON_H_

#include <string>
#include <sstream>
#include <fstream>
#include "json_parser.h"

namespace wfrest
{

class Json
{
public: 
    Json();
    Json(const std::string& str);
    Json(const char* str);

    ~Json();

    static Json parse(const std::string &str);
    static Json parse(const std::ifstream& stream);
    
    const std::string& dump();
    const std::string& dump(int spaces);
private:
    void value_convert(const json_value_t *val, int spaces, int depth, std::string* out_str);

    void string_convert(const char *raw_str, std::string* out_str);

    void number_convert(double number, std::string* out_str);

    void array_convert(const json_array_t *arr, int spaces, int depth, std::string* out_str);

    void array_convert_not_format(const json_array_t *arr, std::string* out_str);

    void object_convert(const json_object_t *obj, int spaces, int depth, std::string* out_str);

    void object_convert_not_format(const json_object_t *obj, std::string* out_str);
private:
    json_value_t *json_ = nullptr;
    std::string str_;
};

}  // namespace wfrest


#endif  // WFREST_JSON_H_
