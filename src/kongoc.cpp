#include "kongoc.h"
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <vcruntime.h>

template<typename Base, typename T>
inline bool instanceof(const T*) {
    return std::is_base_of<Base, T>::value;
}

std::ostream & operator<<(std::ostream& os, Value& value) {
    if(std::holds_alternative<float>(value)) {
        std::cout << std::get<float>(value);
    } else if (std::holds_alternative<bool>(value)) {
        std::cout << (std::get<bool>(value) ? "true" : "false");
    } else if(auto hobj = std::get_if<HeapObj*>(&value)) {
        std::cout << "KHeapObj{}"; 
    }
    return os;
}

BytecodeFile::BytecodeFile(std::vector<uint8_t> bytecode) {}

Instruction into_instruction(uint8_t bc) {
    assert(bc <= 15 && bc >= 0);
    return static_cast<Instruction>(bc);
}

VM::VM() {}

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
   if(auto obj = std::get_if<HeapObj*>(&v)){
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

inline unsigned short parse_ushort (std::vector<uint8_t>& chunk, size_t location) {
    return (chunk[location-1] << 8) | chunk[location];
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

    size_t instr_ptr = 0;
    while(instr_ptr < bytecode.size()) {

        Instruction instruction = static_cast<Instruction>(bytecode[instr_ptr]);
        std::cout << std::setw(2) << instr_ptr << " ";
        switch (instruction) {
            case Instruction::Halt:
                std::cout << "Halt" << std::endl;
                break;
            case Instruction::LoadConst: {
                instr_ptr+=1;
                std::cout << "LoadConst  " 
                    << instr_ptr << " "
                    << values.at(bytecode[instr_ptr]) 
                    << std::endl;
            } break;
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
                std::cout << "DefGlobal " << ++instr_ptr << std::endl;
                break;
            case Instruction::GetGlobal:
                std::cout << "GetGlobal " << ++instr_ptr << std::endl;
                break;
            case Instruction::JmpIfFalse: {
                instr_ptr += 2;
                std::cout << "JmpIfFalse"<< "[" << instr_ptr-1 << "," << instr_ptr << "]";
                auto jump = parse_ushort(bytecode, instr_ptr);
                std::cout << " " << jump+instr_ptr << "\n";
            } break;
            case Instruction::Jmp: {
                instr_ptr += 2;
                std::cout << "Jmp" << "[" << instr_ptr-1 << "," << instr_ptr << "]";
                auto jump = parse_ushort(bytecode, instr_ptr);
                std::cout << " " << jump+instr_ptr << "\n";
                //instr_ptr += jump;
            } break;
            default:
                std::cout << "Unknown instruction " << instr_ptr << std::endl;
                break;
        }
        instr_ptr += 1;
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
                instr_ptr += 1;
                auto word = words.at(chunk.at(instr_ptr));
                auto val = stck.top(); stck.pop();
                //todo fix: word location
                globals.insert({ word, val });
            } break;
            case Instruction::GetGlobal: {
                instr_ptr += 1;
                auto name_idx = chunk.at(instr_ptr);
                auto word = words.at(name_idx);
                stck.push(globals.at(word));
            } break;
            case Instruction::JmpIfFalse: {
                instr_ptr += 2;
                auto jump = parse_ushort(chunk, instr_ptr);
                auto val = stck.top(); stck.pop();
                if(auto b = std::get_if<bool>(&val)) {
                   if(!*b) { 
                       instr_ptr+=jump; 
                   }
                }
            } break;
            case Instruction::Jmp: {
                instr_ptr += 2;
                auto jump = parse_ushort(chunk, instr_ptr);
                instr_ptr += jump;
            }
        }
        instr_ptr += 1;
    }
    return 0;
}
