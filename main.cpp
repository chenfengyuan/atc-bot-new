#include <iostream>
#include <fstream>
#include <string>
#include "rapidjson/document.h"
#include "atc.hpp"
#include "gtest/gtest.h"
#include "atc-utils.hpp"
#include "atc_search.hpp"
#include <queue>
#include <functional>

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
    atc::plane p({5,5,atc::direction::d}, 0, 0, 3, dest_, 100);
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

TEST(atc, read_status){
    std::string data = R"json({"update_time": 1403772825.5098014, "data": "30 21 5\n8 12 0 4 29 0 5 29 7 6 29 17 6 9 20 1 0 13 2 0 7 2 0 0 3 \n2 20 15 0 20 18 2 \n31\n0 0 24 5 7 7 2 36 1\n1 0 12 14 7 1 2 37 4\n2 1 10 13 7 0 3 41 2\n3 0 27 2 7 0 3 49 5\n\n"})json";
    atc_utils::frame rv = atc_utils::read_status(data);
    std::ostringstream out;
    out << rv;
    char const * output = "update_time : 1.40377e+09\nclck : 31\n{map 30x21,\nexits:{ {dest {position 0, 0, {dir c } }, exit(7) }, {dest {position 0, 7, {dir d } }, exit(6) }, {dest {position 0, 13, {dir d } }, exit(5) }, {dest {position 9, 20, {dir e } }, exit(4) }, {dest {position 29, 17, {dir a } }, exit(3) }, {dest {position 29, 7, {dir a } }, exit(2) }, {dest {position 29, 0, {dir z } }, exit(1) }, {dest {position 12, 0, {dir x } }, exit(0) },  },\nairports:{ {dest {position 20, 18, {dir d } }, airport(1) }, {dest {position 20, 15, {dir w } }, airport(0) },  },\nplanes:{ {plane D7, {position 27, 2, {dir z } }, {dest {position 20, 15, {dir w } }, airport(0) }, 49 }, {plane c7, {position 10, 13, {dir d } }, {dest {position 20, 15, {dir w } }, airport(0) }, 41 }, {plane B7, {position 12, 14, {dir x } }, {dest {position 29, 0, {dir z } }, exit(1) }, 37 }, {plane A7, {position 24, 5, {dir e } }, {dest {position 0, 0, {dir c } }, exit(7) }, 36 },  } }\n";
    EXPECT_EQ(output, out.str());
}

TEST(atc, search_node){
    std::priority_queue<atc_search::search_node> a;
    atc_search::search_node node{atc::plane(), 0};
    node.score = 1.0;
    a.push(node);
    node.score = 1.1;
    a.push(node);
    node.score = 0.9;
    a.push(node);
    std::ostringstream out;
    while(!a.empty()){
        auto e = a.top();
        a.pop();
        out << e.score << ", ";
    }
    EXPECT_EQ(out.str(), "0.9, 1, 1.1, ");

    atc::dest exit0(atc::position(5,5,atc::direction::w), atc::dest::exit, 0);
    atc::dest airport0(atc::position(5,5,atc::direction::w), atc::dest::airport, 0);
    atc_search::search_node node0(atc::plane(atc::position(5,5, atc::direction::c), 1, 0, 2, airport0, 100), 0);
    node0.calculate_heuristic_estimate(airport0);
    EXPECT_EQ(node0.is_finished(airport0), false);
    EXPECT_EQ(node0.heuristic_estimate, 4);
    node0.calculate_heuristic_estimate(exit0);
    EXPECT_EQ(node0.is_finished(exit0), false);
    EXPECT_EQ(node0.heuristic_estimate, 7);
    atc_search::search_node node1(atc::plane(atc::position(0,0, atc::direction::c), 1, 0, 2, airport0, 100), 0);
    node1.calculate_heuristic_estimate(airport0);
    EXPECT_EQ(node1.is_finished(airport0), false);
    EXPECT_EQ(node1.heuristic_estimate, 8);
    node1.calculate_heuristic_estimate(exit0);
    EXPECT_EQ(node1.is_finished(exit0), false);
    EXPECT_EQ(node1.heuristic_estimate, 7);
}
TEST(atc, search){
    const char * data = R"json({"update_time": 1404197019.1171052, "data": "30 21 5\n8 12 0 4 29 0 5 29 7 6 29 17 6 9 20 1 0 13 2 0 7 2 0 0 3 \n2 20 15 0 20 18 2 \n2\n0 0 12 5 7 1 2 50 5\n1 0 12 5 9 1 2 50 5\n\n"})json";
    // two plane aiport
    data = R"json({"update_time": 1404293195.5471077, "data": "30 21 1\n4 29 7 6 29 17 6 0 7 2 0 0 3 \n1 20 18 2 \n3337\n11 0 27 17 7 0 3 49 6\n13 1 29 7 7 0 3 51 6\n\n"})json";
    {
        atc_utils::frame fm = atc_utils::read_status(data);
        EXPECT_EQ(atc_search::search(fm).size(), 2);
    }
    data = R"json({"update_time": 1404311151.5923011, "data": "30 21 1\n2 13 0 4 0 10 2 \n7 6 5 0 23 4 6 19 7 0 25 10 2 24 16 6 13 13 4 6 13 0 \n6\n0 1 13 6 7 2 3 45 4\n1 0 23 4 0 2 3 51 6\n\n"})json";
    {
        atc_utils::frame fm = atc_utils::read_status(data);
        EXPECT_EQ(atc_search::search(fm).size(), 1);
    }
    data = R"json({"update_time": 1404317726.3879964, "data": "30 21 1\n2 13 0 4 0 10 2 \n7 6 5 0 23 4 6 19 7 0 25 10 2 24 16 6 13 13 4 6 13 0 \n7\n6 0 10 2 1 1 3 51 2\n\n"})json";
    {
        atc_utils::frame fm = atc_utils::read_status(data);
        EXPECT_EQ(atc_search::search(fm).size(), 1);
    }
    data = R"json({"update_time": 1404351974.9621122, "data": "30 21 1\n2 13 0 4 0 10 2 \n7 6 5 0 23 4 6 19 7 0 25 10 2 24 16 6 13 13 4 6 13 0 \n47\n0 1 11 4 3 3 3 45 2\n1 1 10 12 1 2 3 46 2\n2 0 23 9 2 3 3 41 3\n3 0 15 12 3 2 3 41 2\n4 1 23 9 4 3 3 32 3\n6 0 23 16 1 5 3 50 6\n8 0 14 14 1 3 3 49 2\n9 1 16 4 5 0 2 39 7\n10 1 19 9 1 1 3 37 1\n11 0 22 5 2 3 3 49 4\n12 0 12 7 1 1 2 35 5\n13 0 11 4 1 1 3 45 2\n15 0 23 11 1 6 3 46 5\n17 0 20 13 1 4 3 36 1\n18 1 9 10 7 2 3 42 2\n19 0 26 11 1 2 3 49 4\n20 0 15 4 1 1 2 38 5\n21 1 13 3 7 6 3 48 4\n22 0 17 2 1 1 2 40 5\n23 0 19 4 3 0 2 47 6\n24 1 17 16 1 6 3 44 6\n25 0 25 8 3 3 3 42 2\n5 1 23 4 0 2 3 51 6\n7 1 25 10 0 1 2 51 2\n14 0 25 10 0 1 3 51 2\n16 0 6 13 0 2 3 51 0\n\n"})json";
    std::ifstream file{"test"};
    std::string data_string;
    std::getline(file, data_string);
    atc_utils::frame fm = atc_utils::read_status(data_string.c_str());
    fm.map.mark_position(atc::position(20,15,0),22,1);
    std::cout << fm;
    for(auto & pair : atc_search::search(fm)){
        std::cout << pair.first << "\n";
        for(auto & pair2 : pair.second){
            std::cout << pair2.second << "\n";
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

