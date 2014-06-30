#include <iostream>
#include <string>
#include "rapidjson/document.h"
#include "atc.hpp"
#include "gtest/gtest.h"
#include "atc-utils.hpp"

using namespace std;
TEST(atc, direction) {
    atc::direction d1(atc::direction::a);
    atc::direction d2(6);
    EXPECT_EQ(d1,d2);
    for(int i=0;i<8;++i){
        atc::direction d1(i);
        atc::direction d2(i);
        EXPECT_EQ(d1, d2);

        atc::direction d3(i);
        atc::direction d4 = d1.get_contary_direction();
        EXPECT_TRUE(atc::direction::is_contary_direction(d3, d4));

        atc::direction d5(i);
        d5.turn_hard_left();
        d5.turn_hard_right();
        EXPECT_TRUE(d5 == atc::direction(i));

        atc::direction d6(i);
        atc::direction d7(i);
        atc::direction d8(i);
        for(int j=0;j<8;++j){
            d7.turn_left();
            d8.turn_right();
        }
        EXPECT_TRUE(d6 == d7);
        EXPECT_TRUE(d6 == d8);

        atc::direction d9(i);
        atc::direction d10(i);
        atc::direction d11(i);
        for(int i=0;i<4;++i){
            d10.turn_hard_left();
            d11.turn_hard_right();
        }
        EXPECT_TRUE(d9 == d10);
        EXPECT_TRUE(d9 == d11);
    }
}
TEST(atc, position){
    atc::position p1(1,1,1);
    atc::position p2(1,1,1);
    atc::position p3(p2);
    atc::position p4,p5(0,0,0);
    atc::position p6;
    EXPECT_TRUE(p1==p2);
    EXPECT_TRUE(p1 == p3);
    EXPECT_TRUE(p4 == p5);
    p6=p1;
    EXPECT_TRUE(p6==p1);

    atc::position p7{1,1,1};
    p7.move();
    atc::position p8{2,0,1};
    EXPECT_TRUE(p7 == p8);
}
TEST(atc, plane){
    atc::dest dest_{atc::position{0,0,3}, atc::dest::airport, 0};
    atc::plane p({5,5,atc::direction::d}, 0, 0, 3, dest_);
    auto ps = p.get_next_positions();
    EXPECT_TRUE(ps[0] == atc::position({6, 5, atc::direction::d}));
    EXPECT_TRUE(ps[1] == atc::position({5, 4, atc::direction::w}));
    EXPECT_TRUE(ps[2] == atc::position({5, 6, atc::direction::x}));
    EXPECT_TRUE(ps[3] == atc::position({6, 4, atc::direction::e}));
    EXPECT_TRUE(ps[4] == atc::position({6, 6, atc::direction::c}));
}
TEST(atc, dest){
    atc::dest dest_{atc::position{0,0,3}, atc::dest::airport, 0};
    assert(dest_ == dest_);
}

TEST(atc, gamemap){
    atc::game_map gm{10,10};
    atc::dest d{{1,1,1}, atc::dest::exit, 0};
    gm.add_exit(d);
    EXPECT_EQ(gm.get_exit(0), d);
    gm.mark_position(atc::position(5,5,0), 10, 5);
    EXPECT_EQ(true, gm.is_safe(atc::position(5,5), 9, 5));
    EXPECT_EQ(false, gm.is_safe(atc::position(4,4), 10, 5));
    EXPECT_EQ(false, gm.is_safe(atc::position(5,6), 10, 5));
    EXPECT_EQ(true, gm.is_safe(atc::position(5,7), 10, 5));
    EXPECT_EQ(true, gm.is_safe(atc::position(5,5), 10, 3));
}

int main(int argc, char **argv)
{
    std::string data = R"json({"update_time": 1403772825.5098014, "data": "30 21 5\n8 12 0 4 29 0 5 29 7 6 29 17 6 9 20 1 0 13 2 0 7 2 0 0 3 \n2 20 15 0 20 18 2 \n31\n0 0 24 5 7 7 2 36 1\n1 0 12 14 7 1 2 37 4\n2 1 10 13 7 0 3 41 2\n3 0 27 2 7 0 3 49 5\n\n"})json";
    atc_utils::frame rv = atc_utils::read_status(data);
    std::cout << rv;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());
    cout << doc["update_time"].GetDouble() << "\n";
    cout << doc["data"].GetString() << "\n";
    cout << data << endl;
    atc::direction d1(3);
    atc::direction d2(3);
    std::cout << (d1 == d2.get_contary_direction() ) << "\n";
    return 0;
}

