#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "templatedb/db.hpp"
#include "utils/Value.hpp"

class DBTest : public ::testing::Test
{
protected:
    templatedb::DB db0;
    templatedb::DB db1;
    templatedb::DB db2;

    Value v1 = Value({1, 2});
    Value v2 = Value({6, 10});
    Value v3 = Value({1, 1, 5, 7});
    Value v4 = Value({13, 176});


    void SetUp() override 
    {
        // TODO: each db should have a config.txt
        db1.put(2, v1);
        db1.put(5, v2);
        db2.put(1024, v3);
    }
};


TEST_F(DBTest, IsEmptyInitially)
{
    EXPECT_EQ(db0.size(), 0);
}


TEST_F(DBTest, GetFunctionality)
{
    Value val = db1.get(2);
    EXPECT_EQ(DBTest::v1, val);
}


TEST_F(DBTest, PutAndGetFunctionality)
{
    db0.put(10, v4);
    Value get_value = db0.get(10);

    EXPECT_EQ(DBTest::v4, get_value);
}


TEST_F(DBTest, DeleteFunctionality)
{
    db1.del(2);
    EXPECT_EQ(db1.get(2), Value(false));
    EXPECT_EQ(db1.size(), 1);

    db2.del(1024);
    EXPECT_EQ(db2.get(1024), Value(false));
    EXPECT_EQ(db2.size(), 0);
}


TEST_F(DBTest, ScanFunctionality)
{
    std::vector<Value> vals;
    vals = db2.scan();
    ASSERT_EQ(vals.size(), 1);
    EXPECT_EQ(vals[0], DBTest::v3);

    vals = db1.scan(1, 3);
    ASSERT_EQ(vals.size(), 1);
    EXPECT_EQ(vals[0], DBTest::v1);

    vals = db1.scan();
    ASSERT_EQ(vals.size(), 2);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}