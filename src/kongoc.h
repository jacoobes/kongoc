#include <stack>
#include <vector>
#include <string>
#include <variant>
#include <functional>


using Value = std::variant<bool, float>;

std::ostream& operator<<(std::ostream& os, Value& value); 

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
    DefGlobal   , // define a global variable
    GetGlobal   ,  
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
   size_t add_value (Value v);
   size_t add_word (std::string const& wrd);
   uint8_t global_idx(std::string const& wrd);
   std::vector<Value> values;
private:
   std::unordered_map<std::string, size_t> globals;
   std::vector<std::string> words;
};
