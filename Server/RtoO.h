#include <iostream>

#ifndef RtoO
#define RtoO

// 지역 공통 클래스
class Region {
public:

    // 기본 변수
    int clon, clat, angle;
    std::string tempa, tempb, tempc, tempd, tempe, tempf, tempg, temph;
    std::string info, entertime, origin, recordtime, color, nextinfo;
    int slon, slat;
    std::string name, situation;
    
    // 생성자 함수
    Region(int clon, int clat, int angle, std::string info, std::string entertime, std::string origin, std::string color, std::string nextinfo)
        : clon(clon), clat(clat), angle(angle), info(info), entertime(entertime), origin(origin), color(color), nextinfo(nextinfo) {}

    // 메세지 처음부분을 제작하는 함수
    std::string MessageStart(){
        std::string write = "";
        write += "Info : " + info + "\n";
        write += "EnterTime : " + entertime + "\n";
        write += "Origin : " + origin + "\n";
        write += "RecordTime : " + recordtime + "\n";
        write += "Color : " + color + "\n";
        write += "NextInfo : " + nextinfo + "\n\n";
        return write;
    }

    // 메세지 본문을 제작하는 함수
    std::string MessageBuild(){
        std::string write = "";
        write += "경도 : " + std::to_string(slon).insert(3, ".") + "\n";
        write += "위도 : " + std::to_string(slat).insert(2, ".") + "\n";
        write += "명칭 : " + name + "\n";
        write += "상황 : " + situation + "\n";
        return write;
    }   

};

//////////////////////////////

// 서울 정리 함수
void Seoul_Intersection(Region& Seoul, std::string seoul_intersection_raw);
void Seoul_LightSignal(Region& Seoul, std::string seoul_lightsignal_raw);
void Seoul_LightTime(Region& Seoul, std::string seoul_lighttime_raw);
void Seoul_Velocity(Region& Seoul, std::string seoul_velocity_raw, std::string seoul_node_raw);
void Seoul_Emergency(Region& Seoul, std::string seoul_emergency_raw);

// 대구 정리 함수
void Daegu_Emergency(Region& Daegu, std::string daegu_emergency_raw);

#endif