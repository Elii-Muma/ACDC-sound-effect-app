#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <libloaderapi.h>
#include <filesystem>

class Database{

    private:
        
    public:
        
        std::string dbName;

        Database();
        std::string getExecutablePath();
        int createDb(const char* name);
        int createTable(const char* name);
        int insertData(const char* dbName, std::string path, std::string name, std::string option, std::string pType);
        std::string initSelect(const char* name, std::string arg1, std::string arg2);
        std::string selectPath(const char* name, std::string arg1);
        int updateToUnchosen(const char* name);
        int updateToChosen(const char* name, std::string arg1, std::string arg2);
        int updatePowerType(const char* name, std::string arg1, std::string arg2);
        int updateToNone(const char* name);
        std::vector<std::string> selectData(const char* name, std::string colName);


};