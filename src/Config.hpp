#ifndef Config_hpp
#define Config_hpp

#include <filesystem>
#include <vector>
#include <string>
namespace fs = std::filesystem;
//===PATH_DIR================================================
// prepare the directory to store the blocks
static fs::path CURRENT_PATH = fs::current_path();
static std::vector<std::string> DB_PATH_DIR = {"Data", "DataBase"};
//=== DATA TYPE ===============================================
using TYPE_INDEX = unsigned long long;
using TYPE_DATA = long long int;
struct TYPE_POS_MAP {
    TYPE_INDEX row_index;
    TYPE_INDEX col_index; 
};
//=== PARAMETERS ABOUT CONNECTION =============================
static std::string SERVER_IP = "localhost";
static std::string SERVER_PORT = "5555";
/* COMMANDS */
static std::string COMMAND_SEND_DB = "send_db";
static std::string COMMAND_SUCCESS = "success";
#endif