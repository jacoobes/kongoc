#include "kongoc.h"
#include <cassert>
#include <fstream>
#include <iostream>


std::ostream & operator<<(std::ostream& os, Value& value) {
    if(std::holds_alternative<float>(value)) {
        std::cout << std::get<float>(value);
    } else if (std::holds_alternative<bool>(value)) {
        std::cout << std::get<bool>(value);
    }
    return os;
}

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


size_t VM::add_value(Value v) {
   values.push_back(v); 
   return values.size()-1;
}


size_t VM::add_word (std::string const& wrd) {
   words.push_back(wrd); 
   return words.size()-1;
}

uint8_t VM::global_idx(std::string const& wrd) {
    if(globals.count(wrd)) {
        return globals.at(wrd);
    } else {
        std::cout << "Could not find global "<< wrd << "\n";
        exit(1);
    }
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


void VM::dump(std::vector<uint8_t> bytecode) {
    std::cout << "DUMP:\n";
    std::cout << "Values: " << "(size="<<values.size()<<")\n";
    for(auto& v: values) {
        std::cout << "v: ";
        std::cout << v << " ";
    }
    std::cout <<"\n";
    std::cout << "Globals: \n";
    for(auto& [k, v]: globals) {
        std::cout << k << ":" << v; 
    }

    std::cout << "\n";
    for (size_t i = 0; i < bytecode.size(); ++i) {
        Instruction instruction = static_cast<Instruction>(bytecode[i]);
        switch (instruction) {
            case Instruction::Halt:
                std::cout << "Halt" << std::endl;
                break;
            case Instruction::LoadConst:
                std::cout << "LoadConst" << std::endl;
                break;
            case Instruction::Negate:
                std::cout << "Negate ";
                std::cout << values.back();
                break;
            case Instruction::Add:
                std::cout << "Add" << std::endl;
                i+=1;
                break;
            case Instruction::Sub:
                std::cout << "Sub" << std::endl;
                i+=1;
                break;
            case Instruction::Mul:
                std::cout << "Mul" << std::endl;
                i+=1;
                break;
            case Instruction::Div:
                std::cout << "Div" << std::endl;
                i+=1;
                break;
            case Instruction::Mod:
                std::cout << "Mod" << std::endl;
                i+=1;
                break;
            case Instruction::Lte:
                std::cout << "Lte" << std::endl;
                i+=1;
                break;
            case Instruction::Not:
                std::cout << "Not" << std::endl;
                break;
            case Instruction::And:
                std::cout << "And" << std::endl;
                break;
            case Instruction::Or:
                std::cout << "Or" << std::endl;
                break;
            case Instruction::DefGlobal:
                i+=1;
                std::cout << "DefGlobal" << std::endl;
                break;
            case Instruction::GetGlobal:
                std::cout << "GetGlobal" << std::endl;
                break;
            default:
                std::cout << "Unknown instruction" << std::endl;
                break;
        }
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
            case Instruction::Halt: {
                dump(chunk);
                return Success;
            } break;
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
            case Instruction::Lte: {
                auto r = values.back(); values.pop_back();
                auto l = values.back(); values.pop_back();
                auto rf = std::get_if<float>(&r),
                     lf = std::get_if<float>(&l);
                //if both are floats
                if(rf && lf) {
                   values.push_back(*lf <= *rf);
                } else {
                   values.push_back(false);
                }
            } break;
            case Instruction::And:
                // Implement as needed
                break;
            case Instruction::Or: {
                auto r = values.back(); values.pop_back();
                auto l = values.back(); values.pop_back();
                auto rf = std::get_if<bool>(&r),
                     lf = std::get_if<bool>(&l);
                //if both are bools
                if(rf && lf) {
                   values.push_back(*lf || *rf);
                } else {
                   values.push_back(false);
                }
            } break;
            case Instruction::DefGlobal: {
                instr_ptr += 1;
                auto val_idx = chunk.at(instr_ptr);
                globals.insert({ (words.at(val_idx)), val_idx });
            } break;
            case Instruction::GetGlobal: {
                instr_ptr += 1;
                auto name_idx = chunk.at(instr_ptr);
                auto word = words.at(name_idx);
                values.push_back(values.at(globals.at(word)));
            } break;
        }
        instr_ptr += 1;
    }
    return 0;
}
