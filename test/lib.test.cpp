#include "catch.hpp"
#include "kongoc.h"
#include <variant>



TEST_CASE("negate number") { 
    VM vm;
    size_t f = vm.add_value(4.f);
    vm.interp_chunk({ 
            static_cast<uint8_t>(Instruction::Negate),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.values.back()));    
    REQUIRE(std::get<float>(vm.values.back()) == -4.f);    
}


TEST_CASE("inverse bool") { 
    VM vm;
    size_t f = vm.add_value(false);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Not),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<bool>(vm.values.back()));    
    REQUIRE(std::get<bool>(vm.values.back()) == true);    
}


TEST_CASE("add") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Add),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.values.back()));    
    REQUIRE(std::get<float>(vm.values.back()) == 3.f);    
}


TEST_CASE("sub") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Sub),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.values.back()));    
    REQUIRE(std::get<float>(vm.values.back()) == -1.f);    
}


TEST_CASE("mult") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Mul),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.values.back()));    
    REQUIRE(std::get<float>(vm.values.back()) == 2.f);    
}

TEST_CASE("div") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Div),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.values.back()));    
    REQUIRE(std::get<float>(vm.values.back()) == 0.5f);    
}
