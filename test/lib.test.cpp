#include "catch.hpp"
#include "lib.h"
#include <variant>

TEST_CASE("Read version") {
    BytecodeFile bc({ 127, 'K', 'G', 'C', '2',   });
    REQUIRE(bc.version == std::string({ 127, 'K', 'G', 'C', '2' }));
}
