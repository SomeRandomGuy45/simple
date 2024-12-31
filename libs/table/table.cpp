#include "table.h"

void Table::addItem(const std::string& name, const std::string& value) {
    tableItems.emplace(name, TableObj(name, value));
}

open DLLEXPORT ReturnType helper new_table(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "[TABLE] Error: Invalid number of arguments\n";
        return nullptr;
    }
    Table created_table;
    tableList[args[0]] = created_table;
    return nullptr;
}

open DLLEXPORT ReturnType helper get_table_item(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "[TABLE] Error: Invalid number of arguments\n";
        return nullptr;
    }
    if (tableList.count(args[0]) == 0) {
        std::cerr << "[TABLE] Error: Table \"" << args[0] << "\" not found\n";
        return nullptr;
    }
    Table& table = tableList[args[0]];
    if (table.tableItems.count(args[1]) == 0) {
        std::cerr << "[TABLE] Error: Item \"" << args[1] << "\" not found in table \"" << args[0] << "\"\n";
        return nullptr;
    }
    return table.tableItems[args[1]].returnValue();
}

open DLLEXPORT ReturnType helper new_table_object(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "[TABLE] Error: Invalid number of arguments\n";
        return nullptr;
    }
    if (tableList.count(args[0]) == 0) {
        std::cerr << "[TABLE] Error: Table \"" << args[0] << "\" not found\n";
        return nullptr;
    }
    Table& table = tableList[args[0]];
    table.addItem(args[1], args[2]);
    return nullptr;
}

open DLLEXPORT std::vector<std::string> helper listFunctions() {
    return {"new_table", "get_table_item", "new_table_object"};
}

open DLLEXPORT FunctionPtr helper getFunction(const char* name) {
    if (std::string(name) == "new_table") {
        return &new_table;
    } else if (std::string(name) == "get_table_item") {
        return &get_table_item;
    } else if (std::string(name) == "new_table_object") {
        return &new_table_object;
    }
    return nullptr;
}