// 라이브러리
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <ctime>

// 헤더파일
#include "RtoO.h"

//////////////////////////////
//////////////////////////////

// 돌발
void Daegu_Emergency(Region& Daegu, std::string daegu_emergency_raw){

    // 필요 변수 제작
    Daegu.tempa = "";
    size_t startPos = 0;
    size_t endPos = 0;
    int unitvector[2];
    unitvector[0] = static_cast<int>(round(10*cos(Daegu.angle*M_PI/180)));
    unitvector[1] = static_cast<int>(round(10*sin(Daegu.angle*M_PI/180)));
    int unitvector_lv[2];
    unitvector_lv[0] = static_cast<int>(round(10*cos((Daegu.angle+90)*M_PI/180)));
    unitvector_lv[1] = static_cast<int>(round(10*sin((Daegu.angle+90)*M_PI/180)));
    int unitvector_rv[2];
    unitvector_rv[0] = static_cast<int>(round(10*cos((Daegu.angle+270)*M_PI/180)));
    unitvector_rv[1] = static_cast<int>(round(10*sin((Daegu.angle+270)*M_PI/180)));       
    int vertex_x[4] = {Daegu.clon+50*unitvector_lv[0], Daegu.clon+50*unitvector_rv[0], Daegu.clon+50*unitvector_lv[0]+200*unitvector[0], Daegu.clon+50*unitvector_rv[0]+200*unitvector[0]};
    int vertex_y[4] = {Daegu.clat+50*unitvector_lv[1], Daegu.clat+50*unitvector_rv[1], Daegu.clat+50*unitvector_lv[1]+200*unitvector[1], Daegu.clat+50*unitvector_rv[1]+200*unitvector[1]};
    std::string minX = std::to_string(*std::min_element(vertex_x, vertex_x + 4)).insert(3, ".");
    std::string maxX = std::to_string(*std::max_element(vertex_x, vertex_x + 4)).insert(3, ".");
    std::string minY = std::to_string(*std::min_element(vertex_y, vertex_y + 4)).insert(2, ".");
    std::string maxY = std::to_string(*std::max_element(vertex_y, vertex_y + 4)).insert(2, ".");

    // 데이터 돌면서 메세지 제작
    while( (startPos=daegu_emergency_raw.find("\"INCIDENTTITLE\":\"", startPos)) != std::string::npos ){

        startPos += 17; // 상황
        endPos = daegu_emergency_raw.find("\"", startPos);
        Daegu.situation = daegu_emergency_raw.substr(startPos, endPos-startPos);

        startPos = daegu_emergency_raw.find("\"COORDX\":", startPos); // 경도
        startPos += 9;
        endPos = daegu_emergency_raw.find(",", startPos);
        Daegu.tempb = daegu_emergency_raw.substr(startPos, endPos-startPos);
        if( !((std::stof(minX) <= std::stof(Daegu.tempb)) && (std::stof(Daegu.tempb) <= std::stof(maxX))) ){
            continue;
        }        

        startPos = daegu_emergency_raw.find("\"COORDY\":", startPos); // 위도
        startPos += 9;
        endPos = daegu_emergency_raw.find(",", startPos);
        Daegu.tempc = daegu_emergency_raw.substr(startPos, endPos-startPos); 
        if( !((std::stof(minY) <= std::stof(Daegu.tempc)) && (std::stof(Daegu.tempc) <= std::stof(maxY))) ){
            continue;
        }

        Daegu.tempa += "경도 : "+Daegu.tempb+"\n"; // 메세지
        Daegu.tempa += "위도 : "+Daegu.tempc+"\n";
        Daegu.tempa += "명칭 : null\n";
        Daegu.tempa += "상황 : "+Daegu.situation+"\n\n";   

    }

    // 데이터가 있었는지 없었는지 판단
    if(Daegu.tempa == "") Daegu.name = "none";
    else{
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
        int roundedMinutes = (localTime->tm_min / 5) * 5;
        std::string zeroaddH, zeroaddM;
        if(localTime->tm_hour < 10) zeroaddH = "0";
        if(roundedMinutes < 10) zeroaddM = "0";
        Daegu.recordtime = zeroaddH + std::to_string(localTime->tm_hour) + ":" + zeroaddM + std::to_string(roundedMinutes) + ":00";
    }

}