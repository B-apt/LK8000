#include <gtest/gtest.h>

TEST(test_suite_sample, dummy_test) {
    std::cout << "This is a message printed using std::cout" << std::endl;
    ASSERT_TRUE(true);
}

// MAIN function for googletest
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}