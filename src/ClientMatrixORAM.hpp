# ifndef ClientMatrixORAM_hpp
# define ClientMatrixORAM_hpp
#include "Config.hpp"
#include <string>
class ClientMatrixORAM {
private:


public:
    ClientMatrixORAM();
    ~ClientMatrixORAM();
    int initialize();
    int access(TYPE_INDEX blockID, string op, TYPE_DATA& data); 
};
# endif