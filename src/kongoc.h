#include <forward_list>
#include <stack>
#include <stdint.h>
#include <vector>
#include <array>
#include <string>
#include <variant>
#include <functional>

enum class OTag {
    String,
    Custom
};

class HeapObj{
public:
    virtual std::string to_string() = 0;
    virtual ~HeapObj() = 0;
};

class KString: public HeapObj {
private: 
    std::string* __internal;
public:
    KString(const std::string& str);
    ~KString() override;
    std::string to_string() override;
    size_t length();
    const char* chars();
};

class KFunction: public HeapObj {
public:
    int arity{};
    ~KFunction() override;
    std::string to_string() override;
    std::string* name = nullptr;
};

using Value = std::variant<bool, float, HeapObj*>;

std::ostream& operator<<(std::ostream& os, Value& value); 

enum class Instruction {
    Halt        , // End execution 
    LoadConst   , // load constant  1 operand
    Negate      , // negate a number
    Add         , // add two values
    Sub         , // subtract two numbers
    Mul         , // multiply two numbers
    Div         , // divide two numbers
    Mod         , // modulus two numbers
    Lte         , 
    Not         , // IfStringEquals
    And         , // IfBoEq
    Or          , // Or boolean operator
    DefGlobal   , // define a global variable
    GetGlobal   , //  1 operand
    DefLocal    ,
    GetLocal    ,
    JmpIfFalse  , // 1 operands, 2 bytes
    Jmp         , // 1 operand, 2 bytes
    Pop         , // Pop one value off the stack.
    Pop_N_Local , // Pop 1-255 elements off the local registers
};




class VM {
public:
   VM();
   ~VM();
   int interp_chunk(std::vector<uint8_t> chunk); 
   void dump(std::vector<uint8_t> chunk);
   size_t add_value(Value v);
   size_t add_word(std::string const&);
   //size_t add_local(Local);
   std::vector<Value> values;
   std::stack<Value> stck;
   std::vector<std::string> words;
private:
   std::unordered_map<std::string, Value> globals;
   std::vector<Value> locals;
   std::forward_list<HeapObj*> objs;
};
