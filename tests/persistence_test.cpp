#include <string>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "templatedb/db.hpp"
#include "utils/Value.hpp"

TEST(PersistenceTest, BasicOpenClose)
{
    std::string fname = "config";
    templatedb::DB db;
    Value v1({1, 2});
    Value v2({2,3,4});
    db.open(fname);
    db.put(5, v1);
    db.put(3, v1);
    db.put(10, v2);
    db.put(15, v1);
    db.close();

    db.open(fname);
    Value v_get1 = db.get(5);
    Value v_get2 = db.get(10);
    db.close();
    remove(fname.c_str());
    ASSERT_EQ(v1, v_get1);
    ASSERT_EQ(v2, v_get2);
}

TEST(PersistenceTest, DeleteOpenClose)
{
    std::string fname = "config1";
    templatedb::DB db;
    Value v1({1, 2});
    Value v2({12, 21});
    db.open(fname);
    db.put(5, v1);
    db.put(1, v2);
    db.del(5);
    db.close();

    db.open(fname);
    Value v3 = db.get(5);
    Value v4 = db.get(1);
    db.close();
    remove(fname.c_str());
    ASSERT_EQ(v3, Value(false));
    ASSERT_EQ(v2, v4);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}