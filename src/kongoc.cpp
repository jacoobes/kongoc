#include "kongoc.h"
#include <cassert>
#include <fstream>
#include <iostream>


std::ostream & operator<<(std::ostream& os, Value& value) {
    if(std::holds_alternative<float>(value)) {
        std::cout << std::get<float>(value);
    } else if (std::holds_alternative<bool>(value)) {
        std::cout << std::get<bool>(value) ? "true" : "false";
    } else if(std::holds_alternative<HeapString*>(value)) {
        std::cout << "\"" << std::get<HeapString*>(value)->chars << "\"";
    }
    return os;
}

BytecodeFile::BytecodeFile(std::vector<uint8_t> bytecode) {}

Instruction into_instruction(uint8_t bc) {
    assert(bc <= 13 && bc >= 0);
    return static_cast<Instruction>(bc);
}


VM::VM() {

}

VM::~VM() {
    for (auto it = objs.begin(); it != objs.end(); ++it) {
        delete *it;
        //it = objs.erase_after(it);  
    }
}

enum StatusCode {
  Success,
  Fail
};


size_t VM::add_value(Value v) {
   if(auto obj = std::get_if<HeapString*>(&v)){
      objs.push_front(*obj);
   }
   values.push_back(v); 
   return values.size()-1;
}


size_t VM::add_word (std::string const& wrd) {
   words.push_back(wrd); 
   return words.size()-1;
}


template <typename BinaryOp>
inline void binary_math_op(BinaryOp fn, VM* vm) {
    auto r = vm->stck.top(); vm->stck.pop();
    auto l = vm->stck.top(); vm->stck.pop();
    if(auto lef = std::get_if<float>(&l);
       auto right = std::get_if<float>(&r)) {
       auto new_val = std::invoke(fn, *lef, *right);
       vm->stck.push(new_val);
    } else {
        printf("Failed to call binary op: mismatch types");
        exit(1);
    }
}


void VM::dump(std::vector<uint8_t> bytecode) {
    std::cout << "DUMP:\n";
    std::cout << "Values: " << "(size="<<values.size()<<")= [";
    for(auto& v: values) {
        std::cout << v << ",";
    }
    std::cout << "]\n";
    std::cout << "Globals: \n";
    for(auto& [k, v]: globals) {
        std::cout << k << ":" << v << ", "; 
    }
    std::cout << "\n";
    std::cout << "Stack: " << "(size="<<stck.size()<<")= [";
    while(!stck.empty()) {
        auto v = stck.top();
        stck.pop();
        std::cout << v << ",";
    }
    std::cout << "]\n";
    for (size_t i = 0; i < bytecode.size(); ++i) {
        Instruction instruction = static_cast<Instruction>(bytecode[i]);
        switch (instruction) {
            case Instruction::Halt:
                std::cout << "Halt" << std::endl;
                break;
            case Instruction::LoadConst:
                std::cout << "LoadConst  " << values.at(bytecode[++i]) << std::endl;
                break;
            case Instruction::Negate: {
                std::cout << "Negate";
            } break;
            case Instruction::Add:
                std::cout << "Add" << std::endl;
                break;
            case Instruction::Sub:
                std::cout << "Sub" << std::endl;
                break;
            case Instruction::Mul:
                std::cout << "Mul" << std::endl;
                break;
            case Instruction::Div:
                std::cout << "Div" << std::endl;
                break;
            case Instruction::Mod:
                std::cout << "Mod" << std::endl;
                break;
            case Instruction::Lte:
                std::cout << "Lte" << std::endl;
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
                std::cout << "DefGlobal" << std::endl;
                break;
            case Instruction::GetGlobal:
                std::cout << "GetGlobal" << std::endl;
                i++;
                break;
            case Instruction::JmpIfFalse: {
                std::cout << "JmpIfFalse" << std::endl;
                i += 2;
            } break;
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
                stck.push(values.at(chunk.at(instr_ptr)));
            } break;
            case Instruction::Halt: {
                dump(chunk);
                return Success;
            } break;
            case Instruction::Negate: 
              if(auto top = std::get_if<float>(&stck.top())) {
                  stck.pop();
                  stck.push(-*top);
              } else {
                printf("Cannot negate non number"); 
                exit(1);
              } break;
            case Instruction::Not: 
              if(auto top = std::get_if<bool>(&stck.top())) {
                  stck.pop();
                  stck.push(!*top);
              } else {
                printf("Cannot negate boolean"); 
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
                auto r = stck.top(); stck.pop();
                auto l = stck.top(); stck.pop();
                auto rf = std::get_if<float>(&r),
                     lf = std::get_if<float>(&l);
                //if both are floats
                if(rf && lf) {
                   stck.push(*lf <= *rf);
                } else {
                   stck.push(false);
                }
            } break;
            case Instruction::And: {
                auto r = stck.top(); stck.pop();
                auto l = stck.top(); stck.pop();
                auto rf = std::get_if<bool>(&r),
                     lf = std::get_if<bool>(&l);
                //if both are bools
                if(rf && lf) {
                   stck.push(*lf && *rf);
                } else {
                   stck.push(false);
                }
            } break;
            case Instruction::Or: {
                auto r = stck.top(); stck.pop();
                auto l = stck.top(); stck.pop();
                auto rf = std::get_if<bool>(&r),
                     lf = std::get_if<bool>(&l);
                //if both are bools
                if(rf && lf) {
                   stck.push(*lf || *rf);
                } else {
                   stck.push(false);
                }
            } break;
            case Instruction::DefGlobal: {
                auto val = stck.top(); stck.pop();
                //todo fix: word location
                globals.insert({ words.back(), val });
            } break;
            case Instruction::GetGlobal: {
                instr_ptr += 1;
                auto name_idx = chunk.at(instr_ptr);
                auto word = words.at(name_idx);
                stck.push(globals.at(word));
            } break;
            case Instruction::JmpIfFalse: {
                instr_ptr += 2;
                unsigned short jmp_offset = 
                    chunk.at(instr_ptr-2) | chunk.at(instr_ptr-1);
            } break;
        }
        instr_ptr += 1;
    }
    return 0;
}
