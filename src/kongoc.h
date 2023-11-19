#include <vector>
#include <string>
#include <variant>
#include <functional>


using Value = std::variant<std::nullptr_t, bool, float, std::string>;

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

};
