#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <variant>

#define open extern "C"
#ifdef _WIN32
    #define DLLEXPORT __declspec(dllexport)
    #define helper __stdcall
#else
    #define DLLEXPORT __attribute__((visibility("default")))
    #define helper
#endif

using ReturnType = std::variant<std::nullptr_t, std::string>;

// Function pointer type for library functions
typedef ReturnType(*FunctionPtr)(const std::vector<std::string>& args);

class TableObj {
public:
    TableObj() = default;
    TableObj(std::string name, std::string value) : name(name), value(value) {};
    void setValue(const std::string& newValue) { value = newValue; }
    void setName(const std::string& newName) { name = newName; }
    std::string returnValue() const { return value; }
    std::string returnName() const { return name; }
private:
    std::string name;
    std::string value;
};

class Table {
public:
    Table() = default;
    Table(const std::string& tableName) : tableName(tableName) {};
    void addItem(const std::string& name, const std::string& value);
    std::unordered_map<std::string, TableObj> tableItems;
private:
    std::string tableName;
};

std::unordered_map<std::string, Table> tableList;

// Function to list all available functions in the library
open DLLEXPORT std::vector<std::string> helper listFunctions();

// Function to get a function pointer by name
open DLLEXPORT FunctionPtr helper getFunction(const char* name);

#endif // TABLE_H