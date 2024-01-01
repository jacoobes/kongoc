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
    REQUIRE(std::holds_alternative<float>(vm.stck.top()));    
    REQUIRE(std::get<float>(vm.stck.top()) == -4.f);    
}


TEST_CASE("inverse bool") { 
    VM vm;
    size_t f = vm.add_value(false);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Not),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<bool>(vm.stck.top()));    
    REQUIRE(std::get<bool>(vm.stck.top()) == true);    
}


TEST_CASE("add") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Add),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.stck.top()));    
    REQUIRE(std::get<float>(vm.stck.top()) == 3.f);    
}


TEST_CASE("sub") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Sub),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.stck.top()));    
    REQUIRE(std::get<float>(vm.stck.top()) == -1.f);    
}


TEST_CASE("mult") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Mul),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.stck.top()));    
    REQUIRE(std::get<float>(vm.stck.top()) == 2.f);    
}

TEST_CASE("div") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::Div),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<float>(vm.stck.top()));    
    REQUIRE(std::get<float>(vm.stck.top()) == 0.5f);    
}

TEST_CASE("alloc string") { 
    VM vm;
    auto heap_str = new HeapString{.length=4};
    heap_str->chars=strdup("deez");
    size_t right = vm.add_value(heap_str);    
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(std::holds_alternative<HeapString*>(vm.stck.top()));    
    REQUIRE(strcmp(std::get<HeapString*>(vm.stck.top())->chars, "deez"));    
}