#include "catch.hpp"
#include "kongoc.h"
#include <cstring>
#include <variant>


//Tests broken atm due to change in how instructions work.
TEST_CASE("negate number") { 
    VM vm;
    size_t f = vm.add_value(4.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::LoadConst),
        0,
        static_cast<uint8_t>(Instruction::Negate),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(as_double(vm.stck.top()) == -4.f);    
}


TEST_CASE("inverse bool") { 
    VM vm;
    size_t f = vm.add_value(0.f);
    vm.interp_chunk({
        static_cast<uint8_t>(Instruction::LoadConst),
        0,
        static_cast<uint8_t>(Instruction::Not),
    });
    REQUIRE(vm.values.size() == 1);    
    REQUIRE(as_double(vm.stck.top()) == true);    
}


TEST_CASE("add") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::LoadConst),
        0,
        static_cast<uint8_t>(Instruction::LoadConst),
        1,
        static_cast<uint8_t>(Instruction::Add),
    });
    REQUIRE(as_double(vm.stck.top()) == 3.f);    
}


TEST_CASE("sub") { 
    VM vm;
//    size_t right = vm.add_value(1.f);
//    size_t left = vm.add_value(2.f);
//    vm.interp({ 
//        static_cast<uint8_t>(Instruction::Sub),
//    });
    //REQUIRE(vm.values.size() == 1);    
    //REQUIRE(as_double(vm.stck.top()) == -1.f);    
}


TEST_CASE("mult") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::LoadConst),
        0,
        static_cast<uint8_t>(Instruction::LoadConst),
        1,
        static_cast<uint8_t>(Instruction::Mul),
    });
    REQUIRE(vm.values.size() == 2);    
    REQUIRE(as_double(vm.stck.top()) == 2.f);    
}

TEST_CASE("div") { 
    VM vm;
    size_t right = vm.add_value(1.f);
    size_t left = vm.add_value(2.f);
    vm.interp_chunk({ 
        static_cast<uint8_t>(Instruction::LoadConst),
        0,
        static_cast<uint8_t>(Instruction::LoadConst),
        1,
        static_cast<uint8_t>(Instruction::Div),
    });
    REQUIRE(as_double(vm.stck.top()) == 0.5f);    
}

TEST_CASE("alloc:string") { 
    VM vm;
    auto heap_str = new KString("deez");
    size_t right = vm.add_value(heap_str);
    REQUIRE(vm.values.size() == 1);
    REQUIRE(as_heapobj(vm.values[0]) != nullptr);
    REQUIRE(std::strcmp(static_cast<KString*>(as_heapobj(vm.values[0]))->chars(), "deez") == 0);
}


TEST_CASE("alloc:func") {
    VM vm;
    auto fn = new KFunction("test");
    vm.add_value(fn);
    REQUIRE(static_cast<KFunction*>(as_heapobj(vm.values[0])) != nullptr);
}



