#include "kongoc.h"
#include <cassert>
#include <fstream>
#include <iostream>




BytecodeFile::BytecodeFile(std::vector<uint8_t> bytecode) {

}

enum class Instruction {
    Halt        , // End execution 
    LoadConst   , // load constant
    Negate      , // negate a number
    Add         , // add two values
    Sub         , // subtract two numbers
    Mul         , // multiply two numbers
    Div         , // divide two numbers
    Mod         , // modulus two numbers
    Lte,
    Not         , // IfStringEquals
    And         , // IfBoEq
    Or          , // Or boolean operator
};

Instruction into_instruction(uint8_t bc) {
    assert(bc <= 13 && bc >= 0);
    return static_cast<Instruction>(bc);
}


VM::VM() {

}

VM::~VM() {

}
enum StatusCode {
  Success,
  Fail

};
int VM::interp_chunk(std::vector<uint8_t> chunk){ 
    size_t instr_ptr = 0;
    while (instr_ptr < chunk.size()) {
        uint8_t i = chunk.at(instr_ptr);
        Instruction instruction = into_instruction(i);

        switch (instruction) {
            case Instruction::LoadConst: {
                instr_ptr += 1;
                break;
            }
            case Instruction::Halt:
                return Success;
            case Instruction::Negate: {

            } break;
            case Instruction::Not: {
                break;
            }

            case Instruction::Add: {
                break;
            }
            case Instruction::Sub:
                // Implement as needed
                break;
            case Instruction::Mul:
                // Implement as needed
                break;
            case Instruction::Div:
                // Implement as needed
                break;
            case Instruction::Mod:
                // Implement as needed
                break;
            case Instruction::Lte:
                // Implement as needed
                break;
            case Instruction::And:
                // Implement as needed
                break;
            case Instruction::Or:
                // Implement as needed
                break;
        }
        instr_ptr += 1;
    }
    return 0;
}
