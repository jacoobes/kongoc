#include <forward_list>
#include <stack>
#include <stdint.h>
#include <vector>
#include <array>
#include <string>
#include <variant>
#include <functional>

#define FRAMES_MAX 64

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

struct expr;
//struct expr_list {
//  struct expr* hd;
//  struct expr_list* tl;
//};
enum ValueTag {
    NUMBER,
    OBJECT
};
struct Value {
  ValueTag tag;
  union {
    double floatv;
    HeapObj* obj;
    //struct expr_list list;
  };
  Value(double num) : tag(ValueTag::NUMBER), floatv(num) {}
  Value(const Value& other) : tag(other.tag) {
        switch (tag) {
            case ValueTag::NUMBER:
                floatv = other.floatv;
                break;
            case ValueTag::OBJECT:
                obj = other.obj;
                break;
        }
    }
    Value(Value&& other) noexcept : tag(other.tag) {
        switch (tag) {
            case ValueTag::NUMBER:
                floatv = other.floatv;
                break;
            case ValueTag::OBJECT:
                obj = other.obj;
                other.obj = nullptr; // Set the moved-from obj to nullptr
                break;
        }
    }
};
double as_double(const Value& v); 
class KFunction: public HeapObj {
public:
    int arity{};
    ~KFunction() override;
    KFunction(std::string const&);
    KFunction(std::string const&, std::vector<uint8_t>, std::vector<Value>);
    std::string to_string() override;
    std::string name = "";
    std::vector<uint8_t> chunk;
    std::vector<Value> locals;
};

struct CallFrame {
    KFunction* kfn;
    //Beginning to the ip
    uint8_t* ip;
    std::vector<Value> slots;
};

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
    Call        ,
    Return      ,
};



class VM {
   std::unordered_map<std::string, Value> globals;
   std::vector<Value> locals;
   std::forward_list<HeapObj*> objs;
   std::array<CallFrame, FRAMES_MAX> frames; 
public:
   VM();
   ~VM();
   int interp_chunk(std::vector<uint8_t> const&);
   void dump(std::vector<uint8_t> chunk);
   size_t add_value(Value v);
   size_t add_word(std::string const&);
   std::vector<Value> values;
   std::stack<Value> stck;
   std::vector<std::string> words;

};



