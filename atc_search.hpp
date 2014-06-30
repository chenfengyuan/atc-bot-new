#ifndef ATC_SEARCH_H
#define ATC_SEARCH_H
#include "atc.hpp"
namespace atc_search{
struct search_node{
    atc::position pos;
    int altitude;
    int fuel;
    double score;
    int distince;
    int heuristic_estimate;
};
bool operator> (search_node const & n1, search_node const & n2){
    return n1.score > n2.score;
}
}

#endif // ATC_SEARCH_H
