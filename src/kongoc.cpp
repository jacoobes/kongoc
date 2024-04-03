#include "kongoc.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdint.h>

bool holds_alternative(Value v, ValueTag vt) {
    return v.tag  == vt;
}

double as_double(const Value& v) {
    assert(v.tag == ValueTag::NUMBER);
    return v.floatv;
}



std::ostream & operator<<(std::ostream& os, Value& value) {
    if(holds_alternative(value, ValueTag::NUMBER)) {
        os << value.floatv;
    }  else if(holds_alternative(value, ValueTag::OBJECT)) {
        os << value.obj->to_string();
    }
    return os;
}

HeapObj::~HeapObj(){};

KString::KString(const std::string& str) {
    __internal = new std::string(std::move(str));
}
KString::~KString() {
    delete __internal;
}
std::string KString::to_string() {
    return *__internal;
}

const char* KString::chars() {
    return __internal->c_str();
}

size_t KString::length() {
    return __internal->size();
}

std::string KFunction::to_string() {
    std::stringstream ss{};
    ss << "(" 
       << (name == "" ? "<anonymous>" : name) 
       << " " << arity << ")";
    return ss.str();
}

KFunction::~KFunction(){}
KFunction::KFunction(std::string const& _name): name(_name){}
KFunction::KFunction(
        std::string const& _name,
        std::vector<uint8_t> bytecode,
        std::vector<Value> _locals): name(_name), chunk(bytecode), locals(_locals){}

Instruction into_instruction(uint8_t bc) {
    assert(bc <= 19 && bc >= 0);
    return static_cast<Instruction>(bc);
}



VM::VM() {}

VM::~VM() {
    for (auto it = objs.begin(); it != objs.end(); ++it) {
        delete *it;
    }
}


enum StatusCode {
  Success,
  Fail
};


size_t VM::add_value(Value v) {
    if(holds_alternative(v, ValueTag::OBJECT)){
        objs.push_front(v.obj);
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
     if(holds_alternative(l, ValueTag::NUMBER) &&
        holds_alternative(r, ValueTag::NUMBER)) {
       auto new_val = std::invoke(fn, as_double(l), as_double(r));
       vm->stck.push(new_val);
    } else {
        printf("Failed to call binary op: mismatch types");
        exit(1);
    }
}

inline unsigned short parse_ushort (std::vector<uint8_t> const& chunk, size_t location) {
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
            case Instruction::Halt: std::cout << "Halt" << std::endl; break;
            case Instruction::LoadConst: {
                instr_ptr+=1;
                std::cout << "LoadConst  " 
                    << instr_ptr << " "
                    << values.at(bytecode[instr_ptr]) 
                    << std::endl;
            } break;
            case Instruction::Negate: { std::cout << "Negate"; } break;
            case Instruction::Add: { std::cout << "Add" << std::endl; } break;
            case Instruction::Sub: std::cout << "Sub" << std::endl; break;
            case Instruction::Mul: std::cout << "Mul" << std::endl; break;
            case Instruction::Div: std::cout << "Div" << std::endl; break;
            case Instruction::Mod: std::cout << "Mod" << std::endl; break;
            case Instruction::Lte: std::cout << "Lte" << std::endl; break;
            case Instruction::Not: std::cout << "Not" << std::endl; break;
            case Instruction::And: std::cout << "And" << std::endl; break;
            case Instruction::Or: std::cout << "Or" << std::endl; break;
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
            case Instruction::Pop: { std::cout << "Pop"; } break;
            case Instruction::DefLocal: { std::cout << "DefLocal "  << std::endl; } break;
            case Instruction::GetLocal: { std::cout << "GetLocal " << ++instr_ptr << std::endl; } break;
            case Instruction::Pop_N_Local: { std::cout << "Pop_N_Local " << ++instr_ptr << std::endl; } break;
            case Instruction::Call:
                break;
            case Instruction::Return:
                break;
            }
        instr_ptr += 1;
    }
}




int VM::interp_chunk(std::vector<uint8_t> const& chunk) {
    size_t instr_ptr = 0;
    while (instr_ptr < chunk.size()) {
        uint8_t i = chunk.at(instr_ptr);
        Instruction instruction = into_instruction(i);
        switch (instruction) {
            case Instruction::LoadConst: {
                instr_ptr += 1;
                stck.push(values.at(chunk.at(instr_ptr)));
            } break;
            case Instruction::Halt: { dump(chunk); return Success; } break;
            case Instruction::Negate: {
                auto top = stck.top();
                stck.pop();
                stck.push(-as_double(top));
            } break;
            case Instruction::Not: {
                auto top = stck.top();
                stck.pop();
                stck.push(!as_double(top));
            } break;
            case Instruction::Add: binary_math_op(std::plus<float>(), this); break;
            case Instruction::Sub: binary_math_op(std::minus<float>(), this); break;
            case Instruction::Mul: binary_math_op(std::multiplies<float>(), this); break;
            case Instruction::Div: binary_math_op(std::divides<float>(), this); break;
            case Instruction::Mod:
                // Implement as needed
                break;
            case Instruction::Lte: {
                auto rf = as_double(stck.top()),
                     lf = as_double(stck.top());
                stck.pop();
                stck.pop();
                //if both are floats
                if(rf && lf) {
                   stck.push(lf <= rf);
                } else {
                   stck.push(0);
                }
            } break;
            case Instruction::And: {
                auto r = as_double(stck.top()); stck.pop();
                auto l = as_double(stck.top()); stck.pop();
                //if both are bools
                if(r && l) {
                   stck.push(l && r);
                } else {
                   stck.push(0);
                }
            } break;
            case Instruction::Or: {
                auto r = as_double(stck.top()); stck.pop();
                auto l = as_double(stck.top()); stck.pop();
                //if both are bools
                if(r && l) {
                   stck.push(l || r);
                } else {
                   stck.push(0);
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
                auto val = as_double(stck.top()); stck.pop();
                if(!val) {
                    instr_ptr+=jump;
                }
            } break;
            case Instruction::Jmp: {
                instr_ptr += 2;
                auto jump = parse_ushort(chunk, instr_ptr);
                instr_ptr += jump;
            } break;
            case Instruction::Pop: {
                stck.pop();
            } break;
            case Instruction::DefLocal: {
                auto val = stck.top(); stck.pop();
                locals.push_back(val);
            } break;
            case Instruction::GetLocal: {
                instr_ptr += 1;
                stck.push(locals[chunk.at(instr_ptr)]);
            } break;
            case Instruction::Pop_N_Local: {
                instr_ptr += 1;
                auto n = chunk.at(instr_ptr);
                for(int i = 0; i < n; i++) {
                    locals.pop_back();
                }
                assert(n > 0);
            } break;
            case Instruction::Call:
                break;
            case Instruction::Return:
                break;
            }
        instr_ptr += 1;
    }
    return 0;
}
