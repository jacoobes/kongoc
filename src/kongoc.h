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

void vm (BytecodeFile const& bytecode);
