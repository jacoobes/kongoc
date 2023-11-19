#include "kongoc.h"
#include <cassert>
#include <fstream>




BytecodeFile::BytecodeFile(std::vector<uint8_t> bytecode) {

}

enum class Intruction {
    Halt        , // End execution 
    LoadConst   , // load constant
    Negate      , // negate a number
    Add         , // add two values
    Sub         , // subtract two numbers
    Mul         , // multiply two numbers
    Div         , // divide two numbers
    Mod         , // modulus two numbers
    IfLess      , // IfLessThan
    IfGreater   , // IfGreaterThan
    IfEqual     , // IfEqual
    Not         , // IfStringEquals
    And         , // IfBoEq
    Or          , // Or boolean operator
};

Intruction into_instruction(char bc) {
    assert(bc <= 13 && bc >= 0);
    return static_cast<Intruction>(bc);
}


void vm(BytecodeFile const& bytecode) {
    
}
