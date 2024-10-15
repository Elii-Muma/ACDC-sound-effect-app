#include "TheDatabase.hpp"
#include <iostream>


std::string Database::getExecutablePath(){
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exePath(buffer);

    return exePath.substr(0, exePath.find_last_of("\\"));
}

Database::Database(){
}

int Database::createDb(const char* name){

    sqlite3 *db;
    std::string dbName = getExecutablePath() + " " + name;
    sqlite3_open(dbName.c_str(), &db);
    std::cout<<"current file path: " << getExecutablePath() << std::endl;

    sqlite3_close(db);

    return 0;
}

int Database::createTable(const char* name){
    sqlite3 *db;

    std::string sqlTablbe = "CREATE TABLE IF NOT EXISTS PATHINFO("
                            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "PATH           TEXT NOT NULL, "
                            "TITLE          TEXT NOT NULL,"
                            "OPTIONS        TEXT CHECK(OPTIONS IN('CHOSEN', 'UNCHOSEN')), "
                            "POWER_TYPE     TEXT CHECK(POWER_TYPE IN('PLUGGEDIN', 'PLUGGEDOUT', 'NONE', 'BOTH')) );";


    try{
        int exit = 0;
        std::string dbName = getExecutablePath() + name;
        exit = sqlite3_open(dbName.c_str(), &db);

        char* messageErr;
        exit = sqlite3_exec(db, sqlTablbe.c_str(), NULL, 0, &messageErr);

        if(exit != SQLITE_OK){
            std::cerr << "ErroR creating table" << std::endl;
            sqlite3_free(messageErr);
        }else 
            std::cout<<"Table created"<<std::endl;

        sqlite3_close(db);
    }
    catch(const std::exception& e){
        std::cerr << e.what();
    }
    return 0;
}

int Database::insertData(const char* name, std::string path, std::string title, std::string option, std::string pType){

    sqlite3 *db;
    sqlite3_stmt *stmt;
    char* messageErr;

    std::string dbName = getExecutablePath() + name;
    int exit = sqlite3_open(dbName.c_str(), &db);

    const char* query = "INSERT INTO PATHINFO (PATH, TITLE, OPTIONS, POWER_TYPE) VALUES (?, ?, ?, ?);";
    sqlite3_prepare_v2(db, query, -1, &stmt, 0);

    //bind parameters
    if(sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cout<<"bind error"<<std::endl;
    }
    if(sqlite3_bind_text(stmt, 2, title.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cout<<"bind error"<<std::endl;
    }
    if(sqlite3_bind_text(stmt, 3, option.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cout<<"bind error"<<std::endl;
    }
    if(sqlite3_bind_text(stmt, 4, pType.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cout<<"bind error"<<std::endl;
    }

    //execute
    int notExit = sqlite3_step(stmt);

    if(notExit != SQLITE_DONE){
        std::cerr<<"failed to add record " <<std::endl;
    }else   
        std::cout<<"added record"<<std::endl;

    //clean up
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return 0;
}

std::vector<std::string> Database::selectData(const char* name, std::string colName){

    sqlite3 *db;
    sqlite3_stmt *stmt;

    std::string dbName = getExecutablePath() + name;
    sqlite3_open(dbName.c_str(), &db);
    int err;

    //returns data from a certain column you choose
    std::string query = "SELECT " + colName + " FROM PATHINFO"; 

    err = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if(err != SQLITE_OK){
        std::cerr << "select failed! " << sqlite3_errmsg(db) <<std::endl;
    }else
        printf("succesfully retrieved\n");

    std::vector<std::string> data;
    data.reserve(100);
    int i = 0;
    while((err = sqlite3_step(stmt)) == SQLITE_ROW){
        data.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
        std::cout << "values: " << data.back()<< std::endl;
    } 

    if(err != SQLITE_DONE){
        std::cerr << "select failed! " << sqlite3_errmsg(db) <<std::endl;
    }else
        printf("done!\n");

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return data;
}

int Database::updateToUnchosen(const char* name){

    sqlite3* db;

    std::string sql("UPDATE PATHINFO SET OPTIONS = 'UNCHOSEN'");
    std::string dbName = getExecutablePath() + name;
    int exit = sqlite3_open(dbName.c_str(), &db);
    char* errMessage = nullptr;

    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMessage);

    if(exit != SQLITE_OK){
        std::cerr<<"failed to unchoose: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_free(errMessage);
        sqlite3_close(db);
        return -1;
    }else{
        std::cout<<"unchoose successfully!\n";
    }

    sqlite3_close(db);

    return 0;
}

/*arg1 = OPTIONS[PARAS: 'CHOSEN', 'UNCHOSEN'] 
arg2 = POWER_TYPE[PARAS: 'PLUGGEDIN', 'PLUGGEDOUT', 'NONE', 'BOTH']*/
std::string Database::initSelect(const char* name, std::string arg1, std::string arg2){

    sqlite3* db;
    sqlite3_stmt *stmt;

    //update which one is chosen through the title
    std::string sql2 = "SELECT TITLE FROM PATHINFO WHERE OPTIONS = ? AND POWER_TYPE = ?";
    // std::string sql3 = "UPDATE PATHINFO SET POWER_TYPE = 'PLUGGEDOUT' WHERE TITLE = ?";
    
    std::string dbName = getExecutablePath() + name;

    int exit = sqlite3_open(dbName.c_str(), &db);
    char* errMessage;

    //PREPARE FOR FIRST QUERY
    sqlite3_prepare_v2(db, sql2.c_str(), -1, &stmt, 0);
    //bind parameters
    if(sqlite3_bind_text(stmt, 1, arg1.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cerr<<"bind error"<<sqlite3_errmsg(db) << std::endl;
    }
    if(sqlite3_bind_text(stmt, 2, arg2.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cerr<<"bind error"<<sqlite3_errmsg(db) << std::endl;
    }

    std::string title;
    while((exit = sqlite3_step(stmt)) == SQLITE_ROW){
        title = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        std::cout << "Set Tones: " << title << std::endl;
    } 


    int notExit = sqlite3_step(stmt);
    //execute
    if(notExit != SQLITE_DONE){
        std::cerr<<"failed to select tone " <<sqlite3_errmsg(db) <<std::endl;
    }else   
        std::cout<<"tone set"<<std::endl;
    sqlite3_finalize(stmt);

    //clean up
    sqlite3_close(db);

    return title;
}

std::string Database::selectPath(const char* name, std::string arg1){

    sqlite3* db;
    sqlite3_stmt *stmt;

    //update which one is chosen through the title
    std::string sql2 = "SELECT PATH FROM PATHINFO WHERE OPTIONS = 'CHOSEN' AND POWER_TYPE = ?";
    // std::string sql3 = "UPDATE PATHINFO SET POWER_TYPE = 'PLUGGEDOUT' WHERE TITLE = ?";
    
    std::string dbName = getExecutablePath() + name;

    int exit = sqlite3_open(dbName.c_str(), &db);
    char* errMessage;

    //PREPARE FOR FIRST QUERY
    sqlite3_prepare_v2(db, sql2.c_str(), -1, &stmt, 0);
    //bind parameters
    if(sqlite3_bind_text(stmt, 1, arg1.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cerr<<"bind error"<<sqlite3_errmsg(db) << std::endl;
    }

    std::string path;
    while((exit = sqlite3_step(stmt)) == SQLITE_ROW){
        path = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        std::cout << "got path: " << path << std::endl;
    } 


    int notExit = sqlite3_step(stmt);
    //execute
    if(notExit != SQLITE_DONE){
        std::cerr<<"failed to select tone " <<sqlite3_errmsg(db) <<std::endl;
    }else   
        std::cout<<"tone set"<<std::endl;
    sqlite3_finalize(stmt);

    //clean up
    sqlite3_close(db);

    return path;
}

int Database::updateToChosen(const char* name, std::string arg1, std::string arg2){

    sqlite3* db;
    sqlite3_stmt *stmt;

    //update which one is chosen through the title
    std::string sql = "UPDATE PATHINFO SET OPTIONS = 'CHOSEN' WHERE TITLE IN (?, ?)";
    
    std::string dbName = getExecutablePath() + name;

    int exit = sqlite3_open(dbName.c_str(), &db);
    char* errMessage;

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

    //bind parameters
    if(sqlite3_bind_text(stmt, 1, arg1.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cerr<<"bind error"<<sqlite3_errmsg(db) << std::endl;
    }
    if(sqlite3_bind_text(stmt, 2, arg2.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cerr<<"bind error"<<sqlite3_errmsg(db) << std::endl;
    }

    //execute
    int notExit = sqlite3_step(stmt);

    if(notExit != SQLITE_DONE){
        std::cerr<<"failed to add record " <<std::endl;
    }else   
        std::cout<<"added record"<<std::endl;

    //clean up
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return 0;
}

/*arg1 = plugged in sound, arg2 = plugged out sound*/
/*  pass in title of sound from combobox and type of sound(in or out)
    V2: arg1 = type of sound, arg2 = plugged out sound
    PARAS: 'PLUGGEDIN', 'PLUGGEDOUT', 'NONE', 'BOTH'*/
int Database::updatePowerType(const char* name, std::string arg1,std::string arg2){

    sqlite3* db;
    sqlite3_stmt *stmt;

    //update which one is chosen through the title
    std::string sql2 = "UPDATE PATHINFO SET POWER_TYPE = ? WHERE TITLE = ?";
    // std::string sql3 = "UPDATE PATHINFO SET POWER_TYPE = 'PLUGGEDOUT' WHERE TITLE = ?";
    
    std::string dbName = getExecutablePath() + name;

    int exit = sqlite3_open(dbName.c_str(), &db);
    char* errMessage;

    //PREPARE FOR FIRST QUERY
    sqlite3_prepare_v2(db, sql2.c_str(), -1, &stmt, 0);
    //bind parameters
    if(sqlite3_bind_text(stmt, 1, arg1.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cerr<<"bind error"<<sqlite3_errmsg(db) << std::endl;
    }
    if(sqlite3_bind_text(stmt, 2, arg2.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
        std::cerr<<"bind error"<<sqlite3_errmsg(db) << std::endl;
    }
    int notExit = sqlite3_step(stmt);
    //execute
    if(notExit != SQLITE_DONE){
        std::cerr<<"failed to update record " <<sqlite3_errmsg(db) <<std::endl;
    }else   
        std::cout<<"added record"<<std::endl;
    sqlite3_finalize(stmt);


    sqlite3_close(db);

    return 0;
}

int Database::updateToNone(const char* name){

    sqlite3* db;

    std::string sql("UPDATE PATHINFO SET POWER_TYPE = 'NONE'");
    std::string dbName = getExecutablePath() + name;
    int exit = sqlite3_open(dbName.c_str(), &db);
    char* errMessage;

    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMessage);

    if(exit != SQLITE_OK){
        std::cerr<<"failed to none-ify: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1;
    }else{
        std::cout<<"none-ified successfully!\n";
    }

    sqlite3_close(db);

    return 0;
}