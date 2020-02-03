#include <iostream>

#include "gtest/gtest.h"
#include "templatedb/db.hpp"
#include "templatedb/operation.hpp"


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}