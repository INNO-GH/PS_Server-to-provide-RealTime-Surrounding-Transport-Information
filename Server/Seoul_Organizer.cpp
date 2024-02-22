// 라이브러리
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>

// 헤더파일
#include "RtoO.h"

// 원형
std::string Sentence_to_Signal(std::string sign);

//////////////////////////////
//////////////////////////////

// 신호등

// 교차로
void Seoul_Intersection(Region& Seoul, std::string seoul_intersection_raw){
    
    // 선택될 교차로 경도, 위도, 명칭, 상황 변수 초기화
    Seoul.slon = 126700000, Seoul.slat = 37400000, Seoul.name = "none", Seoul.situation = "none"; 
    
    // 클라이언트 차량 전방으로 1m 간격 200개의 점 찍기
    int unitvector[2];
    unitvector[0] = static_cast<int>(round(10*cos(Seoul.angle*M_PI/180))); // 단위벡터
    unitvector[1] = static_cast<int>(round(10*sin(Seoul.angle*M_PI/180)));
    int clientfront[201][2]; 
    for(int i=0 ; i<201 ; ++i){ // 반복나아가기
        clientfront[i][0] = Seoul.clon + i*unitvector[0];
        clientfront[i][1] = Seoul.clat + i*unitvector[1];
    }

    // API값을 돌면서 값들을 추출해내고, 좌표의 플마 20m 범위 안에 200개의 점 중 포함되는게 있으면, 기존것 비교 가까운것으로 교차로ID로 등록
    size_t startPos=0;
    size_t endPos;

    while( (startPos=seoul_intersection_raw.find("\"itstId\":\"", startPos)) != std::string::npos ){

        startPos += 10;
        endPos = seoul_intersection_raw.find("\"", startPos);
        std::string id = seoul_intersection_raw.substr(startPos, endPos - startPos); // 값추출

        startPos = seoul_intersection_raw.find("\"itstNm\":\"", startPos);
        startPos += 10;
        endPos = seoul_intersection_raw.find("\"", startPos);
        std::string name = seoul_intersection_raw.substr(startPos, endPos - startPos);

        startPos = seoul_intersection_raw.find("\"mapCtptIntLat\":3.", startPos);
        startPos += 18;
        int term_y = std::stoi("3" + seoul_intersection_raw.substr(startPos, 7));

        startPos = seoul_intersection_raw.find("\"mapCtptIntLot\":1.", startPos);
        startPos += 18;
        int term_x = std::stoi("1" + seoul_intersection_raw.substr(startPos, 8));
        
        for(int i=0 ; i<201 ; ++i){
            bool condition1 = ( term_x-200 < clientfront[i][0] ) && ( clientfront[i][0] < term_x+200 ); // 범위내 확인
            bool condition2 = ( term_y-200 < clientfront[i][1] ) && ( clientfront[i][1] < term_y+200 );
            bool condition3 = sqrt(pow(term_x-Seoul.clon, 2)+pow(term_y-Seoul.clat, 2)) < sqrt(pow(Seoul.slon-Seoul.clon, 2)+pow(Seoul.slat-Seoul.clat, 2));
            if( condition1 && condition2 && condition3 ){ // 기존 비교 가까운것
                    Seoul.slon = term_x;
                    Seoul.slat = term_y;
                    Seoul.name = name;
                    Seoul.tempa = id;
                    break;                  
            }
        }

    }

}

//////////

// 신호
void Seoul_LightSignal(Region& Seoul, std::string seoul_lightsignal_raw){
    
    // API를 돌면서, itstId가 있는지 확인
    size_t startPos=0;
    size_t startPos_static;
    size_t endPos;
    bool check = false;

    while( (startPos=seoul_lightsignal_raw.find("\"itstId\":\"", startPos)) != std::string::npos ){
        startPos += 10;
        endPos = seoul_lightsignal_raw.find("\"", startPos);
        std::string id = seoul_lightsignal_raw.substr(startPos, endPos - startPos); // 값추출
        if(Seoul.tempa == id){
            check = true;
            startPos_static = startPos;
            break;
        }
    }

    // 있는경우 - 경도, 위도, 교차로명과 방향에 따른 직진, 좌회전 정보를 기입
    if(check==true){
        std::string direction; // 각도의 따른 신호등의 방향 판단
        if( (27.5<=Seoul.angle) && (Seoul.angle<62.5) ) direction="sw";
        else if( (62.5<=Seoul.angle) && (Seoul.angle<117.5) ) direction="st";
        else if( (117.5<=Seoul.angle) && (Seoul.angle<152.5) ) direction="se";
        else if( (152.5<=Seoul.angle) && (Seoul.angle<207.5) ) direction="et";
        else if( (207.5<=Seoul.angle) && (Seoul.angle<242.5) ) direction="ne";
        else if( (242.5<=Seoul.angle) && (Seoul.angle<297.5) ) direction="nt";
        else if( (297.5<=Seoul.angle) && (Seoul.angle<332.5) ) direction="nw";
        else direction="wt";
        Seoul.tempb = direction;

        std::string sign;

        startPos = startPos_static;
        startPos = seoul_lightsignal_raw.find(direction+"StsgStatNm\":", startPos); // 직진 메세지
        startPos += 14;
        endPos = seoul_lightsignal_raw.find(",", startPos);
        sign = seoul_lightsignal_raw.substr(startPos, endPos - startPos);
        sign = Sentence_to_Signal(sign);
        Seoul.situation = "직진-" + sign + " ";

        startPos = startPos_static;
        startPos = seoul_lightsignal_raw.find(direction+"LtsgStatNm\":", startPos); // 좌회전 메세지
        startPos += 14;
        endPos = seoul_lightsignal_raw.find(",", startPos);
        sign = seoul_lightsignal_raw.substr(startPos, endPos - startPos);
        sign = Sentence_to_Signal(sign);
        Seoul.situation += "좌회전-" + sign + " ";
    }

}

std::string Sentence_to_Signal(std::string sign){ // 문장에 맞는 색신호 리턴하기
    if( sign == "\"protected-Movement-Allowed\"" ){
        return "Green";
    }
    else if( sign == "\"protected-clearance\"" ){
        return "Yellow";
    }
    else if( sign == "\"stop-And-Remain\"" ){
        return "Red";
    }
    else if( sign == "\"permissive-Movement-Allowed\"" ){
        return "GreenFlash";
    }
    else if( sign == "\"permissive-clearance\"" ){
        return "YellowFlash";
    }
    else if( sign == "\"stop-Then-Proceed\"" ){
        return "RedFlash";
    }
    else{
        return "null";
    }
}

//////////

// 잔여시간
void Seoul_LightTime(Region& Seoul, std::string seoul_lighttime_raw){

    std::string time;
    std::string sign;
    size_t startPos;
    size_t endPos;

    startPos = 0;
    startPos = seoul_lighttime_raw.find("\"trsmTm\":\"", startPos); // RecordTime 찾기
    startPos += 10;
    endPos = seoul_lighttime_raw.find("\"", startPos);
    Seoul.recordtime = ((seoul_lighttime_raw.substr(startPos, endPos - startPos)).insert(2, ":")).insert(5, ":");

    startPos = 0;
    startPos = seoul_lighttime_raw.find(Seoul.tempb+"StsgRmdrCs\":", startPos); // 직진 시간 추가
    startPos += 14;
    endPos = seoul_lighttime_raw.find(".", startPos);
    time = seoul_lighttime_raw.substr(startPos, endPos - startPos);
    if(time.length()==1) time.insert(time.length()-1, "0."); 
    else time.insert(time.length()-1, "."); 
    startPos = 0;
    startPos = Seoul.situation.find("직진-", startPos);
    startPos += 7;
    endPos = Seoul.situation.find(" ", startPos);
    sign = Seoul.situation.substr(startPos, endPos - startPos);  
    if(sign != "null"){
        Seoul.situation = Seoul.situation.substr(0,endPos) + "-" + time + "(s)" + Seoul.situation.substr(endPos);
    }

    startPos = 0;
    startPos = seoul_lighttime_raw.find(Seoul.tempb+"LtsgRmdrCs\":", startPos); // 좌회전 시간 추가
    startPos += 14;
    endPos = seoul_lighttime_raw.find(".", startPos);
    time = seoul_lighttime_raw.substr(startPos, endPos - startPos);
    if(time.length()==1) time.insert(time.length()-1, "0."); 
    else time.insert(time.length()-1, "."); 
    startPos = 0;
    startPos = Seoul.situation.find("좌회전-", startPos);
    startPos += 10;
    endPos = Seoul.situation.find(" ", startPos);
    sign = Seoul.situation.substr(startPos, endPos - startPos);  
    if(sign != "null"){
        Seoul.situation = Seoul.situation.substr(0,endPos) + "-" + time + "(s)" + Seoul.situation.substr(endPos);
    }

}

//////////////////////////////
//////////////////////////////

// 통행속도
void Seoul_Velocity(Region& Seoul, std::string seoul_velocity_raw, std::string seoul_node_raw){

    size_t startPos = 0;
    size_t endPos;

    // 데이터가 없는 경우
    if( (seoul_velocity_raw.find("\"roadName\":\"", startPos) == std::string::npos) || (seoul_node_raw.find("\"nodeId\":\"", startPos) == std::string::npos) ) {
        Seoul.name = "none";
        return;
    }

    // 데이터가 있는 경우
    startPos = seoul_velocity_raw.find("\"createdDate\":\"", startPos); // RecordTime 찾기
    startPos += 23;
    endPos = seoul_velocity_raw.find("\"", startPos);
    Seoul.recordtime = ((seoul_velocity_raw.substr(startPos, endPos - startPos)).insert(2, ":")).insert(5, ":");
    startPos = 0;
    while( (startPos=seoul_velocity_raw.find("\"roadName\":\"", startPos)) != std::string::npos ){

        startPos += 12; // 도로명
        endPos = seoul_velocity_raw.find("\"", startPos);
        std::string name = seoul_velocity_raw.substr(startPos, endPos - startPos);

        startPos=seoul_velocity_raw.find("\"startNodeId\":\"", startPos); // 시작노드
        startPos += 15;
        endPos = seoul_velocity_raw.find("\"", startPos);
        std::string depart = seoul_velocity_raw.substr(startPos, endPos - startPos);

        startPos=seoul_velocity_raw.find("\"endNodeId\":\"", startPos); // 종료노드
        startPos += 13;
        endPos = seoul_velocity_raw.find("\"", startPos);
        std::string arrive = seoul_velocity_raw.substr(startPos, endPos - startPos);

        startPos=seoul_velocity_raw.find("\"speed\":\"", startPos); // 속도
        startPos += 9;
        endPos = seoul_velocity_raw.find("\"", startPos);
        std::string speed = seoul_velocity_raw.substr(startPos, endPos - startPos);

        size_t startMat = 0; // 시작노드, 종료노드에 맞는 값 찾기
        size_t endMat = 0;
        bool startCheck = false;
        bool endCheck = false;

        while( (startMat=seoul_node_raw.find("\"nodeId\":\"", startMat)) != std::string::npos ){ // 노드ID 추출
            startMat += 10;
            endMat = seoul_node_raw.find("\"", startMat);

            if(depart == seoul_node_raw.substr(startMat, endMat - startMat)){ // 노드ID와 시작노드ID 비교
                startMat = seoul_node_raw.find("\"x\":\"", startMat);
                startMat += 5;
                Seoul.tempa = seoul_node_raw.substr(startMat, 10);
                
                startMat = seoul_node_raw.find("\"y\":\"", startMat);
                startMat += 5;
                Seoul.tempb = seoul_node_raw.substr(startMat, 9);

                startMat = seoul_node_raw.find("\"nodeName\":\"", startMat);
                startMat += 12;
                endMat = seoul_node_raw.find("\"", startMat);
                Seoul.tempc = seoul_node_raw.substr(startMat, endMat - startMat);

                startCheck = true;
            }

            else if(arrive == seoul_node_raw.substr(startMat, endMat - startMat)){ // 노드ID와 종료노드ID 비교
                startMat = seoul_node_raw.find("\"x\":\"", startMat);
                startMat += 5;
                Seoul.tempd = seoul_node_raw.substr(startMat, 10);
                
                startMat = seoul_node_raw.find("\"y\":\"", startMat);
                startMat += 5;
                Seoul.tempe = seoul_node_raw.substr(startMat, 9);

                startMat = seoul_node_raw.find("\"nodeName\":\"", startMat);
                startMat += 12;
                endMat = seoul_node_raw.find("\"", startMat);
                Seoul.tempf = seoul_node_raw.substr(startMat, endMat - startMat);

                endCheck = true;
            }

            if(startCheck && endCheck) break;
        }

        Seoul.slon = static_cast<int>( (std::stof(Seoul.tempa)*0.75+std::stof(Seoul.tempd)*0.25) * 1e6 ); // 메세지 형성
        Seoul.slat = static_cast<int>( (std::stof(Seoul.tempb)*0.75+std::stof(Seoul.tempe)*0.25) * 1e6 ); 
        Seoul.name = name + "[" + Seoul.tempc + "-" + Seoul.tempf + "]" ;
        Seoul.situation = speed + "(km/h)"; 
        Seoul.tempg += Seoul.MessageBuild() + "\n"; 
    
    } 

}

//////////////////////////////
//////////////////////////////

// 돌발
void Seoul_Emergency(Region& Seoul, std::string seoul_emergency_raw){

    Seoul.tempa = "";
    size_t startPos = 0;
    size_t endPos = 0;

    // 데이터 없는 케이스 처리
    if( (startPos=seoul_emergency_raw.find("\"coordX\":\"", startPos)) == std::string::npos ) {
        Seoul.name = "none";
        return;
    }

    // 데이터 있는 경우 - 경도, 위도, 상황 적기
    while( (startPos=seoul_emergency_raw.find("\"coordX\":\"", startPos)) != std::string::npos ){

        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
        int roundedMinutes = (localTime->tm_min / 5) * 5;
        std::string zeroaddH, zeroaddM;
        if(localTime->tm_hour < 10) zeroaddH = "0";
        if(roundedMinutes < 10) zeroaddM = "0";
        Seoul.recordtime = zeroaddH + std::to_string(localTime->tm_hour) + ":" + zeroaddM + std::to_string(roundedMinutes) + ":00";
        startPos += 10;
        endPos = seoul_emergency_raw.find("\"", startPos);
        Seoul.tempa += "경도 : "+seoul_emergency_raw.substr(startPos, endPos-startPos)+"\n";

        startPos = seoul_emergency_raw.find("\"coordY\":\"", startPos);
        startPos += 10;
        endPos = seoul_emergency_raw.find("\"", startPos);
        Seoul.tempa += "위도 : "+seoul_emergency_raw.substr(startPos, endPos-startPos)+"\n";        

        startPos = seoul_emergency_raw.find("\"message\":\"", startPos);
        startPos += 11;
        endPos = seoul_emergency_raw.find("\"endDate\"", startPos)-2;
        Seoul.tempa += "명칭 : null\n";   
        Seoul.tempa += "상황 : "+seoul_emergency_raw.substr(startPos, endPos-startPos)+"\n\n";   

    }

}