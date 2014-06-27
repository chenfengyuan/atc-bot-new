#ifndef ATC_HPP
#define ATC_HPP
#include <vector>
#include <utility>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <map>
#include <bitset>
namespace atc{
struct direction{
private:
    int code;
public:
    enum {w=0,e=1,d=2,c=3,x=4,z=5,a=6,q=7};
    direction():code{0}{
    }

    direction(int code_):code{code_}{}
    direction(direction const & dir){
        this->code = dir.get_code();
    }
    direction & turn_left(){
        code+=8;
        code--;
        code%=8;
        return *this;
    }

    direction & turn_right(){
        code++;
        code%=8;
        return *this;
    }

    direction & turn_hard_left(){
        code+=8;
        code-=2;
        code%=8;
        return *this;
    }

    direction & turn_hard_right(){
        code += 2;
        code %= 8;
        return *this;
    }

    int get_code() const{
        return code;
    }
    bool static is_same_direction(direction const & a, direction const & b){
        return a.get_code() == b.get_code();
    }
    direction get_contary_direction() const{
        return direction((code+ 4)%8);
    }

    bool static is_contary_direction(direction const &a, direction const &b){
        return is_same_direction(a.get_contary_direction(), b);
    }

    std::pair<int, int> get_position_delta(){
        int dx,dy;
        switch(code){
        case 1:
        case 2:
        case 3:
            dx = 1;break;
        case 0:
        case 4:
            dx = 0;break;
        case 5:
        case 6:
        case 7:
            dx = -1;break;
        default:
            throw std::range_error(std::string("direction's code should between 0 and 7 : current is ") + std::to_string(code));
        }
        switch(code){
        case 3:
        case 4:
        case 5:
            dy = 1;break;
        case 0:
        case 1:
        case 7:
            dy = -1;break;
        case 2:
        case 6:
            dy = 0;break;
        default:
            throw std::range_error(std::string("direction's code should between 0 and 7 : current is ") + std::to_string(code));
        }
        return std::make_pair(dx, dy);
    }
};
bool operator==(direction const &a, direction const &b){
    return direction::is_same_direction(a, b);
}

struct position{
    int x{0}, y{0};
    direction dir{0};
    position(){
    }
    position(int x_, int y_, direction const & dir_):x{x_},y{y_},dir{dir_}{
    }
    position & operator=(const position & o){
        x = o.get_x();
        y = o.get_y();
        dir = o.get_dir();
        return *this;
    }

    int get_x() const{
        return x;
    }
    int get_y() const{
        return y;
    }
    direction get_dir()const{
        return dir;
    }

    position(position const & o):x{o.get_x()},y{o.get_y()},dir{o.get_dir()}{
    }

    position & move(){
        int dx, dy;
        std::tie(dx, dy) = dir.get_position_delta();
        x += dx;
        y += dy;
        return *this;
    }

};
bool operator==(position const & p1, position const & p2){
    return p1.get_dir() == p2.get_dir() &&
            p1.get_x() == p2.get_x() &&
            p1.get_y() == p2.get_y();
}

class plane{
    position pos{};
    bool is_jet{0};
    int num{};
    int altitude;
public:
    plane(position const &pos_,bool is_jet_,int num_, int altitude_):pos{pos_},is_jet{is_jet_},num{num_}, altitude{altitude_}{
    }
    position get_position()const{
        return pos;
    }
    plane(plane const &p):pos{p.get_position()}{}
    std::vector<position> get_next_positions(){
        std::vector<position> ps;
        position p(pos);
        p.move();
        ps.push_back(p);
        auto funcs={&direction::turn_hard_left,&direction::turn_hard_right,&direction::turn_left,&direction::turn_right};
        for(auto func : funcs){
            position p(pos);
            ((&p.dir)->*func)();
            p.move();
            ps.push_back(p);
        }
        return ps;
    }
    bool is_jet_plane(){
        return is_jet;
    }
    int get_no() const{
        return num;
    }
    int get_altitude()const{
        return altitude;
    }
};
class game_map{
    int width{0},height{0};
    std::unordered_map<int, position> exists;
    std::unordered_map<int, position> airports;
    std::unordered_map<int, std::map<int, std::bitset<10>>> points;
public:
    game_map(){}
    game_map(int width_, int height_):width{width_},height{height_}{}
    game_map & add_exists(position const & p,int num){
        exists[num] = p;
        return *this;
    }
    game_map & add_airport(position const &p, int num){
        airports[num] = p;
        return *this;
    }
    position get_exists(int num){
        auto tmp = exists.find(num);
        assert(tmp != exists.end());
        return tmp->second;
    }

    bool is_valid_position(position const &p){
        int x = p.get_x();
        int y = p.get_y();
        return x >= 0 && x < width && y >=0 && y < height;
    }

    bool is_valid_altitude(int altitude){
        return altitude >=0 and altitude <= 9;
    }
    game_map & mark_position(position const &p,int time, int altitude){
        int x = p.get_x();
        int y = p.get_y();
        assert(is_valid_position(p));
        assert(is_valid_altitude(altitude));
        points[x + y * width][time][altitude] = true;
        return *this;
    }
    game_map & mark_position(plane const &p,int time){
        return mark_position(p.get_position(), time, p.get_altitude());
    }
};
}
#endif // ATC_HPP
