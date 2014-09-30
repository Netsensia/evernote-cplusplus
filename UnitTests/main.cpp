//
//  main.cpp
//  UnitTests
//
//  Created by Chris Moreton on 30/09/2014.
//  Copyright (c) 2014 Chris Moreton. All rights reserved.
//

#include <iostream>

class NoteTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        
    }
    
    // virtual void TearDown() {}
    
    Note note1;
};

TEST_F(NoteTest, CanSetValues) {
    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}