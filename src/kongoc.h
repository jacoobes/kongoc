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
    int arity;
    ~KFunction() override;
    std::string* name = nullptr;
    std::string to_string() override;
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
    Pop         , // Pop one value off the stack.
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
