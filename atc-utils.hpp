#ifndef ATCUTILS_HPP
#define ATCUTILS_HPP
#include "atc.hpp"
#include <vector>
#include <sstream>
#include "rapidjson/document.h"
#include <iostream>
namespace atc_utils{
struct frame{
    atc::game_map map;
    double update_time{};
    int clck;
    frame():map{},update_time{},clck{}{}
    frame(frame && f):map{std::move(f.map)},update_time{f.update_time},clck{f.clck}{
    }
    frame & operator<< (frame && f){
        map = std::move(f.map);
        update_time = f.update_time;
        clck = f.clck;
        return *this;
    }
    frame(frame const &)=delete;
    frame & operator=(frame const &)=delete;
};
std::ostream & operator<< (std::ostream & out, frame const &f){
    out << "update_time : " << f.update_time << "\n";
    out << "clck : " << f.clck << "\n";
    out << f.map << "\n";
    return out;
}

frame read_status(std::string raw){
    rapidjson::Document doc;
    doc.Parse<0>(raw.c_str());
    double update_time = doc["update_time"].GetDouble();
    std::istringstream inbuf(doc["data"].GetString());
    int width, height, clck, update_secs;
    inbuf >> width >> height >> update_secs;
    atc::game_map map{width, height};
    int n;
    inbuf >> n;
    for(int i=0;i<n;++i){
        int x,y,dir;
        inbuf >> x >> y >> dir;
        map.add(atc::dest({x,y,dir}, atc::dest::exit, i));
    }
    inbuf >> n;
    for(int i=0;i<n;++i){
        int x,y,dir;
        inbuf >> x >> y >> dir;
        map.add(atc::dest({x,y,dir}, atc::dest::airport, i));
    }
    inbuf.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    inbuf >> clck;
    while(true){
        try{
            int no, type,x,y,altitude,dest_no, dest_type,fuel,dir;
            inbuf >> no >> type >> x >> y >> altitude >> dest_no;
            inbuf >> dest_type >> fuel >> dir;
            atc::dest dest_;
            if(dest_type == atc::dest::airport){
                dest_ = map.get_airport(dest_no);
            }else if(dest_type == atc::dest::exit){
                dest_ = map.get_exit(dest_no);
            }else{
                assert(dest_type == atc::dest::airport||
                        dest_type == atc::dest::exit);
            }
            atc::plane p(atc::position(x,y,dir),type,no,altitude,dest_);
            map.add_plane(std::move(p));
        }catch(const std::ios_base::failure&){
            break;
        }
    }
    frame frame_;
    frame_.map = std::move(map);
    frame_.update_time = update_time;
    frame_.clck = clck;
    return frame_;
}
}
#endif // ATCUTILS_HPP
