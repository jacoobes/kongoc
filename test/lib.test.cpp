#include "catch.hpp"
#include "kongoc.h"
#include <variant>



TEST_CASE("negate number"){ 
    VM vm;
    size_t f = vm.add_const(4.f);
    vm.interp_chunk({ 
            static_cast<uint8_t>(Instruction::Negate),
            static_cast<uint8_t>(f)
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.values.top()));    
    REQUIRE(std::get<float>(vm.values.top()) == -4.f);    
}
