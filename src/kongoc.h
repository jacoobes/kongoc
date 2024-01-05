#include <forward_list>
#include <stack>
#include <vector>
#include <string>
#include <variant>
#include <functional>

enum class OTag {
    String,
    Custom
};

class HeapObj{};

class KString: public HeapObj {
public:
    size_t length;
    char* chars;
};

class KFunction: public HeapObj {

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
    GetGlobal   , //  1 operaand
    JmpIfFalse  , // 1 operands, 2 bytes
    Jmp         , // 1 operand, 2 bytes
};



struct BytecodeFile {
    BytecodeFile(std::vector<uint8_t>);
    std::vector<Value> consts;
    std::vector<uint8_t> bytecode;
    std::string version;
};


class VM {
public:
   VM();
   ~VM();
   int interp_chunk(std::vector<uint8_t> chunk); 
   void dump(std::vector<uint8_t> chunk);
   size_t add_value(Value v);
   size_t add_word(std::string const& wrd);
   std::vector<Value> values;
   std::stack<Value> stck;
   std::vector<std::string> words;
private:
   std::unordered_map<std::string, Value> globals;
   std::forward_list<HeapObj*> objs;
};
