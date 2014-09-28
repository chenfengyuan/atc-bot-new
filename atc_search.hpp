#ifndef ATC_SEARCH_H
#define ATC_SEARCH_H
#include "atc.hpp"
#include "atc-utils.hpp"
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <memory>
#include <thread>
#include <limits>
namespace atc_search{
struct search_node{
    atc::position pos;
    bool is_jet;
    int altitude;
    int fuel;
    double score;
    double distance;
    double heuristic_estimate;
    int id;
    int parent_id;
    int clck;
    std::shared_ptr<int> id_series;
    search_node(){}
    search_node(atc::plane const & p, int clck_):pos{p.get_position()},is_jet{p.is_jet_plane()},altitude{p.get_altitude()},
        fuel{p.get_fuel()}, distance{0}, id{0}, parent_id{0}, clck{clck_}, id_series{new int{}}{
        (*id_series)++;
    }
    search_node(search_node const &n):pos{n.pos},is_jet{n.is_jet},altitude{n.altitude},fuel{n.fuel},score{n.score},distance{n.distance},
            heuristic_estimate{n.heuristic_estimate}, id{n.id}, parent_id{n.parent_id}, clck{n.clck}, id_series{n.id_series}{
    }
    search_node & inherit(){
        clck++;
        if(!is_jet && clck % 2 == 1){
        }else{
            fuel--;
            distance++;
        }
        parent_id = id;
        id = (*id_series)++;
        return *this;
    }

    search_node & calculate_heuristic_estimate(atc::dest const & dest){
        if(dest.airport == dest.dest_type_){
            // 45
            // 30>
            // 21
            atc::position pos_tmp = dest.pos;
            pos_tmp.dir = pos_tmp.dir.get_contary_direction();
            pos_tmp.move();
            heuristic_estimate = std::max(std::abs(pos.x - pos_tmp.x),
                                          std::abs(pos.y - pos_tmp.y)) + 1;
            if(altitude > heuristic_estimate)
                heuristic_estimate = altitude + 3;
            //heuristic_estimate = std::max<double>(heuristic_estimate, altitude);
        }
        if(dest.exit == dest.dest_type_){
            heuristic_estimate = std::max(std::abs(pos.x - dest.pos.x),
                                          std::abs(pos.y - dest.pos.y));
            if(heuristic_estimate <= 10 and altitude != 9)
                heuristic_estimate += 0.1 * (9- altitude);
            if(pos.dir == dest.pos.dir){
                heuristic_estimate += 1;
            }
            heuristic_estimate = std::max<double>(heuristic_estimate, 9 - altitude);
        }
        return *this;
    }
    search_node & calculate_score(){
        score = 5 * heuristic_estimate + distance;
        return *this;
    }

    bool is_finished(atc::dest const & dest){
        if(dest.pos.x == pos.x && dest.pos.y == pos.y)
            ;
        else
            return false;
        if(dest.dest_type_ == dest.airport && dest.pos.dir == pos.dir && altitude == 0)
            return true;
        if(dest.dest_type_ == dest.exit && altitude == 9)
            return true;
        return false;
    }

    std::vector<search_node> get_next_nodes(){
        std::vector<search_node> ns;
        if(!is_jet && (clck % 2 == 0)){
            search_node new_node{*this};
            new_node.inherit();
            ns.push_back(std::move(new_node));
            return ns;
        }else{
            for(int da=-1;da<2;++da){
                search_node new_node{*this};
                new_node.inherit();
                new_node.pos.move();
                new_node.altitude += da;
                ns.push_back(new_node);
                if(this->altitude == 0) // plane can't change direction when in airport
                    continue;
                using atc::direction;
                auto funcs={&direction::turn_hard_left,&direction::turn_hard_right,&direction::turn_left,&direction::turn_right};
                for(auto func : funcs){
                    search_node new_node{*this};
                    new_node.altitude += da;
                    ((&new_node.pos.dir)->*func)();
                    new_node.pos.move();
                    new_node.inherit();
                    ns.push_back(new_node);
                }
            }
        }
        return ns;
    }
    bool is_valid_position(const atc::game_map & map, const atc::dest & dest){
        if(is_finished(dest))
            return true;
        if(pos.x >= 1 and pos.x < map.get_width() - 1 and pos.y >=1 and pos.y < map.get_height() - 1)
            return true;
        return false;
    }
    bool is_valid_altitude(const atc::dest & dest){
        if(is_finished(dest))
            return true;
        if(altitude >= 1 and altitude <=9)
            return true;
        return false;
    }
};
std::ostream & operator<<(std::ostream & out, search_node const & sn){
    out << "{search_node " ;
    out << sn.pos;
    if(sn.is_jet)
        out << " jet ";
    else
        out << " prop ";
    out << "fuel(" << sn.fuel << ") ";
    out << "clck(" << sn.clck << ") ";
    out << "altitude(" << sn.altitude << ") ";
    out << "score(" << sn.score << ") ";
    out << "distince(" << sn.distance << ") ";
    out << "heuristic_estimate(" << sn.heuristic_estimate << ") ";
    out << "id(" << sn.id << ") ";
    out << "parent_id(" << sn.parent_id << ")}";
    return out;
}

bool operator< (search_node const & n1, search_node const & n2){
    return n1.score > n2.score;
}
struct result_node{
    atc::position pos{};
    int altitude{};
    int clck{};
    result_node(){}
    result_node(search_node const & sn):pos{sn.pos}, altitude{sn.altitude}, clck{sn.clck}{
    }
};
std::ostream & operator<<(std::ostream & out, result_node const & rn){
    out << "{ " << rn.pos << " " << rn.altitude  << " " << rn.clck << " }";
    return out;
}

typedef std::map<int, std::map<int, result_node>> search_result;
search_result search(atc_utils::frame & f){
    auto & map = f.map;
    search_result rv;
    std::vector<atc::plane> planes_by_fuel;
    for(auto const & pair:map.get_planes()){
        planes_by_fuel.push_back(pair.second);
        map.mark_position(pair.second, f.clck);
    }
    sort(planes_by_fuel.begin(), planes_by_fuel.end(), [](const atc::plane & a, const atc::plane &b){
        return a.get_fuel() < b.get_fuel();
    });
//    int debug = true;
    int debug = false;
    for(auto const & plane:planes_by_fuel){
//        if(plane.get_no() == 3)
//            debug = true;
        if(debug)
            std::cout << plane.get_no() << "\n";
        std::priority_queue<search_node> ns;
        std::unordered_map<int, search_node> ns_record;
        search_node n{plane, f.clck};
        atc::dest dest = plane.get_dest();
        n.calculate_heuristic_estimate(dest).calculate_score();
        ns.push(n);
        ns_record[n.id] = n;
        std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, bool>>>> uniq_node;
        while(!ns.empty()){
            if(plane.get_altitude() == 0 && ns_record.size() > 42000){
                goto finded;
            }
            if(ns_record.size() > 100000){
                std::cout << "no solution\n";
                std::chrono::milliseconds dura( 200000 );
                std::this_thread::sleep_for( dura );
                break;
            }
            search_node n = ns.top();
            if(debug)
                std::cout << "\n" << n << "\n\n";
            ns.pop();
            for(search_node  nn  : n.get_next_nodes()){
                if(nn.fuel == 1)
                    continue;
                {
                    int offset = map.get_offset(nn.pos.x, nn.pos.y);
                    int dir = nn.pos.dir.get_code();
                    int altitude = nn.altitude;
                    int clck = nn.clck;
                    if(uniq_node[offset][dir][altitude][clck])
                        continue;
                    else
                        uniq_node[offset][dir][altitude][clck] = true;
                }
                if(nn.fuel != map.get_width() + map.get_height()){
                    if(nn.is_valid_altitude(dest) and nn.is_valid_position(map, dest))
                        ;
                    else
                        continue;
                }
                if(nn.altitude <=8 and nn.altitude >=6 and nn.fuel < map.get_width() + map.get_height() - 7){
                    if(nn.pos.x <= 2 || nn.pos.x >= map.get_width() - 4 ||
                            nn.pos.y <= 2 || nn.pos.y >= map.get_height() - 4)
                        continue;
                }
                if(!map.is_safe(nn.pos, nn.clck, nn.altitude))
                    continue;
                nn.calculate_heuristic_estimate(dest).calculate_score();
                if(!(nn.pos.dir == n.pos.dir)){
                    nn.score += 0.01;
                    nn.distance+=0.01;
                }
                if(!(nn.altitude == n.altitude)){
                    nn.score += 0.001;
                    nn.distance += 0.001;
                }
                bool banned = false;
                if(dest.dest_type_ == dest.airport && nn.pos.is_in_front_of(dest.pos)){
                    if(std::max(std::abs(nn.pos.x - dest.pos.x),std::abs(nn.pos.y - dest.pos.y)) <= 1){
                        banned = true;
                    }
                }
                if(!banned)
                    ns.push(nn);
                if(debug && !banned)
                    std::cout << nn << "\n";
                ns_record[nn.id] = nn;
                if(nn.is_finished(dest)){
                    if(debug)
                        std::cout << "nodes " << ns_record.size() << "\n";
                    search_result::value_type::second_type rns;
                    while(true){
                        rns[nn.clck] = nn;
                        map.mark_position(nn.pos, nn.clck, nn.altitude);
                        if(nn.id == nn.parent_id)
                            break;
                        nn = ns_record[nn.parent_id];
                    }
                    rv[plane.get_no()] = std::move(rns);
                    goto finded;
                }
            }
        }
        finded:;
        if(debug)
            std::cout << "\n\n";
    }
    return rv;
}
}

#endif // ATC_SEARCH_H
