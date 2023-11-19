#include "kongoc.h"
#include <cassert>
#include <fstream>
#include <iostream>




BytecodeFile::BytecodeFile(std::vector<uint8_t> bytecode) {

}

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


size_t VM::add_const(Value v) {
   values.push_back(v); 
   return values.size()-1;
}

template <typename BinaryOp>
inline void binary_math_op(BinaryOp fn, VM* vm) {
    auto r = vm->values.back(); vm->values.pop_back();
    auto l = vm->values.back(); vm->values.pop_back();
    if(auto lef = std::get_if<float>(&l);
       auto right = std::get_if<float>(&r)) {
       auto new_val = std::invoke(fn, *lef, *right);
       vm->values.push_back(new_val);
    } else {
        printf("Failed to call binary op: mismatch types");
        exit(1);
    }
}

int VM::interp_chunk(std::vector<uint8_t> chunk){ 
    size_t instr_ptr = 0;
    while (instr_ptr < chunk.size()) {
        uint8_t i = chunk.at(instr_ptr);
        Instruction instruction = into_instruction(i);
        switch (instruction) {
            case Instruction::LoadConst: {
                instr_ptr += 1;
                if(chunk.size() > instr_ptr) {
                    printf("Failed to read const at index\n");
                    exit(1);
                }
                auto idx = chunk.at(instr_ptr);
                values.push_back(values[idx]);
            } break;
            case Instruction::Halt:
                return Success;
            case Instruction::Negate: 
              if(auto top = std::get_if<float>(&values.back())) {
                values.back() = -*top;
              } else {
                printf("Cannot negate non number"); 
                exit(1);
              } break;
            case Instruction::Not: 
              if(auto top = std::get_if<bool>(&values.back())) {
                values.back() = !*top;
              } else {
                printf("Cannot negate non number"); 
                exit(1);
              } break;
            break;
            case Instruction::Add: 
                binary_math_op(std::plus<float>(), this);
                break;
            case Instruction::Sub:
                binary_math_op(std::minus<float>(), this);
                break;
            case Instruction::Mul:
                binary_math_op(std::multiplies<float>(), this);
                break;
            case Instruction::Div:
                binary_math_op(std::divides<float>(), this);
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
            case Instruction::DefGlobal: {
                instr_ptr += 1;
                auto name_idx = chunk.at(instr_ptr);
                globals.insert({ std::get<std::string>(values.at(name_idx)), name_idx });
            } break;
            case Instruction::GetGlobal: {
                instr_ptr += 1;
                auto name_idx = chunk.at(instr_ptr);
                values.push_back(values.at(name_idx));
            } break;
        }
        instr_ptr += 1;
    }
    return 0;
}
