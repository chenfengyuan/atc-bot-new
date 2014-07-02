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
#include <cstdlib>
#include <utility>
#include <functional>
#include <cassert>
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
    friend std::ostream & operator<< (std::ostream & out, direction const & dir){
        char c;
        switch(dir.code){
        case direction::w:
            c = 'w';break;
        case direction::e:
            c = 'e';break;
        case direction::d:
            c = 'd';break;
        case direction::c:
            c = 'c';break;
        case direction::x:
            c = 'x';break;
        case direction::z:
            c = 'z';break;
        case direction::a:
            c = 'a';break;
        case direction::q:
            c = 'q';break;
        default:
            c = '*';
            break;
        }
        out << "{dir "<< c << " }";
        return out;
    }
    char get_char(){
        char c;
        switch(code){
        case direction::w:
            c = 'w';break;
        case direction::e:
            c = 'e';break;
        case direction::d:
            c = 'd';break;
        case direction::c:
            c = 'c';break;
        case direction::x:
            c = 'x';break;
        case direction::z:
            c = 'z';break;
        case direction::a:
            c = 'a';break;
        case direction::q:
            c = 'q';break;
        default:
            c = '*';
            break;
        }
        return c;
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
    position(int x_, int y_, direction const & dir_=0):x{x_},y{y_},dir{dir_}{
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
    position & unmove(){
        int dx, dy;
        std::tie(dx, dy) = dir.get_contary_direction().get_position_delta();
        x += dx;
        y += dy;
        return *this;
    }

    friend int get_distance(position const &p1, position const &p2){
        return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
    }
    friend std::ostream & operator<<(std::ostream & out, position const & pos){
        out << "{position " << pos.x << ", " << pos.y << ", " << pos.dir << " }";
        return out;
    }
};
bool operator==(position const & p1, position const & p2){
    return p1.get_dir() == p2.get_dir() &&
            p1.get_x() == p2.get_x() &&
            p1.get_y() == p2.get_y();
}
struct dest{
    atc::position pos{};
    enum dest_type {exit=2,airport=3};
    dest_type dest_type_{};
    int dest_no{};
    dest(){}
    dest(position pos_, dest_type dest_type__, int dest_no_):pos{pos_},dest_type_{dest_type__},dest_no{dest_no_}{}
    friend std::ostream & operator<< (std::ostream & out, dest const & dest_){
        out << "{dest " << dest_.pos << ", ";
        switch(dest_.dest_type_){
        case dest::exit:
            out << "exit(";break;
        case dest::airport:
            out << "airport(";break;
        default:
            out << "unknow(";break;
        }
        out << dest_.dest_no << ") }";
        return out;
    }
    atc::position get_in_position()const{
        atc::position pos = this->pos;
        pos.unmove();
        return pos;
    }
};
bool operator==(dest const &d1, dest const &d2){
    return d1.pos == d2.pos &&
            d1.dest_type_ == d2.dest_type_ &&
            d1.dest_no == d2.dest_no;
}

class plane{
    position pos{};
    bool is_jet{0};
    int num{};
    int altitude;
    dest dest_;
    int fuel;
public:
    plane(){}
    plane(position const &pos_,bool is_jet_,int num_, int altitude_, dest dest__, int fuel_):pos{pos_},is_jet{is_jet_},num{num_}, altitude{altitude_}, dest_{dest__}, fuel{fuel_}{
    }
    plane(plane const & plane_):pos{plane_.pos},is_jet{plane_.is_jet},num{plane_.num},altitude{plane_.altitude},dest_{plane_.dest_},fuel{plane_.fuel}{
    }

    plane & operator=(plane const & plane_){
        pos = plane_.pos;
        is_jet = plane_.is_jet;
        num = plane_.num;
        altitude = plane_.altitude;
        dest_ = plane_.dest_;
        fuel = plane_.fuel;
        return *this;
    }

    position get_position()const{
        return pos;
    }
    dest get_dest() const{
        return dest_;
    }
    int get_fuel() const{
        return fuel;
    }
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
    bool is_jet_plane() const{
        return is_jet;
    }
    int get_no() const{
        return num;
    }
    int get_altitude()const{
        return altitude;
    }
    friend std::ostream & operator<< (std::ostream & out, plane const &p){
        out << "{plane ";
        char c;
        if(p.is_jet)
            c = 'a';
        else
            c = 'A';
        out << static_cast<char>((p.num - 0) + c) << p.altitude << ", " << p.pos << ", ";
        out << p.dest_ << ", " << p.fuel <<" }";
        return out;
    }
};
class game_map{
    int width{0},height{0};
    std::unordered_map<int, dest> exits;
    std::unordered_map<int, dest> airports;
    std::unordered_map<int, std::map<int, std::bitset<10>>> points;
    std::unordered_map<int, plane> planes;
public:
    game_map(){}
    game_map(int width_, int height_):width{width_},height{height_}{}
    game_map(game_map const &)=delete;
    game_map(game_map && gm):width{gm.width}, height{gm.height},exits{std::move(gm.exits)},airports{std::move(gm.airports)},points{std::move(gm.points)},planes{std::move(gm.planes)}{
    }

    game_map & operator=(game_map &&gm){
        width = gm.width;
        height = gm.height;
        exits = std::move(gm.exits);
        airports = std::move(gm.airports);
        planes = std::move(gm.planes);
        points = std::move(gm.points);
        return *this;
    }

    game_map & add(dest const & dest_){
        if(dest_.dest_type_ == dest::airport){
            return add_airport(dest_);
        }else if(dest_.dest_type_ == dest::exit){
            return add_exit(dest_);
        }else{
            assert(dest_.dest_type_ == dest::airport ||
                   dest_.dest_type_ == dest::exit);
        }
        return *this;
    }

    game_map & add_exit(dest const & dest_){
        assert(dest_.dest_type_ == dest::exit);
        exits[dest_.dest_no] = dest_;
        return *this;
    }
    game_map & add_airport(dest const &dest){
        assert(dest.dest_type_ == dest::airport);
        airports[dest.dest_no] = dest;
        return *this;
    }
    game_map & add_plane(plane const &plane_){
        planes[plane_.get_no()] = plane_;
        return *this;
    }

    int get_width()const{
        return width;
    }
    int get_height()const{
        return height;
    }
    dest get_exit(int num){
        auto tmp = exits.find(num);
        assert(tmp != exits.end());
        return tmp->second;
    }
    dest get_airport(int num){
        auto tmp = airports.find(num);
        assert(tmp != airports.end());
        return tmp->second;
    }
    auto get_points() ->decltype(points)&{
        return points;
    }

    bool is_valid_position(position const &p){
        int x = p.get_x();
        int y = p.get_y();
        return x >= 0 && x < width && y >=0 && y < height;
    }
    bool is_valid_altitude(int altitude){
        return altitude >=0 and altitude <= 9;
    }
    int get_offset(int x, int y){
        return x + y * width;
    }

    game_map & mark_position(position const &p,int time, int altitude){
        int x = p.get_x();
        int y = p.get_y();
        assert(is_valid_position(p));
        assert(is_valid_altitude(altitude));
        points[get_offset(x,y)][time][altitude] = true;
        return *this;
    }
    game_map & mark_position(plane const &p,int time){
        return mark_position(p.get_position(), time, p.get_altitude());
    }
    bool is_safe(position const &p,int time, int altitude){
        int x = p.get_x();
        int y = p.get_y();
        for(int dx=-1;dx<2;++dx){
            for(int dy=-1;dy<2;++dy){
                int x_ = x + dx;
                int y_ = y + dy;
                if(!is_valid_position(position(x_,y_,0)))
                    continue;
                auto ps = points[get_offset(x_,y_)][time];
                for(int da=-1;da<2;++da){
                    if(!is_valid_altitude(da + altitude))
                        continue;
                    if(ps.test(da + altitude))
                        return false;
                }
            }
        }
        return true;
    }
    friend std::ostream & operator<< (std::ostream & out, game_map const & map){
        out << "{map " << map.width << "x" << map.height << ",\n";
        out << "exits:{ ";
        for(auto pair:map.exits){
            out << pair.second << ", ";
        }
        out << " },\nairports:{ ";
        for(auto pair :map.airports){
            out << pair.second << ", ";
        }
        out << " },\nplanes:{ ";
        for(auto pair :map.planes){
            out << pair.second << ", ";
        }
        out << " } }";
        return out;
    }
    std::unordered_map<int, atc::plane> const & get_planes() const{
        return planes;
    }
};
}
#endif // ATC_HPP
