// 라이브러리
#include <iostream>
#include <cmath>
#include <algorithm>
#include <curl/curl.h>

// 헤더파일
#include "CtoR.h"
#include "RtoO.h"

// 원형
std::string Seoul(std::string information, int longitude, int latitude, int angle, std::string time);
std::string Daegu(std::string information, int longitude, int latitude, int angle, std::string time);
std::string Else(std::string information, int longitude, int latitude, int angle, std::string time);
std::string APIget(const std::string& url);
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

// 좌표값으로 지역을 선택하여 코드 넘기기
std::string Selector(std::string information, int longitude, int latitude, int angle, std::string time){
    
    // 서울범위
    if( (126764490<longitude && longitude<127183781) && (37428427<latitude && latitude<37701427) ) {
        return Seoul(information, longitude, latitude, angle, time);
    }      

    // 대구범위
    if( (128350048<longitude && longitude<128899331) && (35606538<latitude && latitude<36326406) ) {
        return Daegu(information, longitude, latitude, angle, time);
    } 

    // 그외범위
    else {
        return Else(information, longitude, latitude, angle, time);
    }

}

//////////////////////////////
//////////////////////////////

// 서울
std::string Seoul(std::string information, int longitude, int latitude, int angle, std::string time){

    // 신호등
    if(information == "초기"){
        Region Seoul(longitude, latitude, angle, "신호등", time, "서울교통빅데이터", "#006400", "통행속도");
        std::string seoul_intersection_raw = APIget("http://t-data.seoul.go.kr/apig/apiman-gateway/tapi/v2xCrossroadMapInformation/1.0?apiKey=51266efd-ee52-4bcb-811b-34c6b0e343a7&pageNo=1&numOfRows=1000");
        seoul_intersection_raw += APIget("http://t-data.seoul.go.kr/apig/apiman-gateway/tapi/v2xCrossroadMapInformation/1.0?apiKey=51266efd-ee52-4bcb-811b-34c6b0e343a7&pageNo=2&numOfRows=1000");
        seoul_intersection_raw += APIget("http://t-data.seoul.go.kr/apig/apiman-gateway/tapi/v2xCrossroadMapInformation/1.0?apiKey=51266efd-ee52-4bcb-811b-34c6b0e343a7&pageNo=3&numOfRows=1000");
        Seoul_Intersection(Seoul, seoul_intersection_raw);
        if(Seoul.name == "none") return Seoul.MessageStart() + "전방에 교차로가 없습니다.\n"; 
        std::string seoul_lightsignal_raw = APIget("http://t-data.seoul.go.kr/apig/apiman-gateway/tapi/v2xSignalPhaseInformation/1.0?apiKey=51266efd-ee52-4bcb-811b-34c6b0e343a7&type=json&pageNo=1&numOfRows=1000");
        seoul_lightsignal_raw += APIget("http://t-data.seoul.go.kr/apig/apiman-gateway/tapi/v2xSignalPhaseInformation/1.0?apiKey=51266efd-ee52-4bcb-811b-34c6b0e343a7&type=json&pageNo=2&numOfRows=1000");
        seoul_lightsignal_raw += APIget("http://t-data.seoul.go.kr/apig/apiman-gateway/tapi/v2xSignalPhaseInformation/1.0?apiKey=51266efd-ee52-4bcb-811b-34c6b0e343a7&type=json&pageNo=3&numOfRows=1000");
        Seoul_LightSignal(Seoul, seoul_lightsignal_raw);
        if(Seoul.situation == "none") return Seoul.MessageStart() + Seoul.name + " 교차로의 신호등 정보는 제공되지 않습니다.\n";
        std::string seoul_lighttime_raw = APIget("http://t-data.seoul.go.kr/apig/apiman-gateway/tapi/v2xSignalPhaseTimingInformation/1.0?apiKey=51266efd-ee52-4bcb-811b-34c6b0e343a7&type=json&pageNo=1&numOfRows=1&itstId="+Seoul.tempa);
        Seoul_LightTime(Seoul, seoul_lighttime_raw);
        return Seoul.MessageStart() + Seoul.MessageBuild();
    }

    // 통행속도
    else if(information == "통행속도"){
        Region Seoul(longitude, latitude, angle, "통행속도", time, "국가교통정보센터", "#00008C", "돌발");
        int unitvector[2];
        unitvector[0] = static_cast<int>(round(10*cos(angle*M_PI/180)));
        unitvector[1] = static_cast<int>(round(10*sin(angle*M_PI/180)));
        int unitvector_lv[2];
        unitvector_lv[0] = static_cast<int>(round(10*cos((angle+90)*M_PI/180)));
        unitvector_lv[1] = static_cast<int>(round(10*sin((angle+90)*M_PI/180)));
        int unitvector_rv[2];
        unitvector_rv[0] = static_cast<int>(round(10*cos((angle+270)*M_PI/180)));
        unitvector_rv[1] = static_cast<int>(round(10*sin((angle+270)*M_PI/180)));       
        int vertex_x[4] = {longitude+50*unitvector_lv[0], longitude+50*unitvector_rv[0], longitude+50*unitvector_lv[0]+200*unitvector[0], longitude+50*unitvector_rv[0]+200*unitvector[0]};
        int vertex_y[4] = {latitude+50*unitvector_lv[1], latitude+50*unitvector_rv[1], latitude+50*unitvector_lv[1]+200*unitvector[1], latitude+50*unitvector_rv[1]+200*unitvector[1]};
        std::string minX = std::to_string(*std::min_element(vertex_x, vertex_x + 4)).insert(3, ".");
        std::string maxX = std::to_string(*std::max_element(vertex_x, vertex_x + 4)).insert(3, ".");
        std::string minY = std::to_string(*std::min_element(vertex_y, vertex_y + 4)).insert(2, ".");
        std::string maxY = std::to_string(*std::max_element(vertex_y, vertex_y + 4)).insert(2, ".");
        std::string seoul_velocity_raw = APIget("https://openapi.its.go.kr:9443/trafficInfo?apiKey=f9dc68565c3049f69696c268b537c4ae&type=all&routeNo=all&drcType=all&minX="+minX+"&maxX="+maxX+"&minY="+minY+"&maxY="+maxY+"&getType=json");
        std::string seoul_node_raw = APIget("https://t-data.seoul.go.kr/apig/apiman-gateway/tapi/TopisIccMsNode/1.0?apiKey=51266efd-ee52-4bcb-811b-34c6b0e343a7&startRow=1&rowCnt=10157");
        Seoul_Velocity(Seoul, seoul_velocity_raw, seoul_node_raw);
        if(Seoul.name == "none") return Seoul.MessageStart() + "주변 도로 정보가 없습니다.\n";
        return Seoul.MessageStart() + Seoul.tempg;
    }
    
    // 돌발
    else{
        Region Seoul(longitude, latitude, angle, "돌발", time, "국가교통정보센터", "#B9062F", "엔드");
        int unitvector[2];
        unitvector[0] = static_cast<int>(round(10*cos(angle*M_PI/180)));
        unitvector[1] = static_cast<int>(round(10*sin(angle*M_PI/180)));
        int unitvector_lv[2];
        unitvector_lv[0] = static_cast<int>(round(10*cos((angle+90)*M_PI/180)));
        unitvector_lv[1] = static_cast<int>(round(10*sin((angle+90)*M_PI/180)));
        int unitvector_rv[2];
        unitvector_rv[0] = static_cast<int>(round(10*cos((angle+270)*M_PI/180)));
        unitvector_rv[1] = static_cast<int>(round(10*sin((angle+270)*M_PI/180)));       
        int vertex_x[4] = {longitude+50*unitvector_lv[0], longitude+50*unitvector_rv[0], longitude+50*unitvector_lv[0]+200*unitvector[0], longitude+50*unitvector_rv[0]+200*unitvector[0]};
        int vertex_y[4] = {latitude+50*unitvector_lv[1], latitude+50*unitvector_rv[1], latitude+50*unitvector_lv[1]+200*unitvector[1], latitude+50*unitvector_rv[1]+200*unitvector[1]};
        std::string minX = std::to_string(*std::min_element(vertex_x, vertex_x + 4)).insert(3, ".");
        std::string maxX = std::to_string(*std::max_element(vertex_x, vertex_x + 4)).insert(3, ".");
        std::string minY = std::to_string(*std::min_element(vertex_y, vertex_y + 4)).insert(2, ".");
        std::string maxY = std::to_string(*std::max_element(vertex_y, vertex_y + 4)).insert(2, ".");
        std::string seoul_emergency_raw = APIget("https://openapi.its.go.kr:9443/eventInfo?apiKey=f9dc68565c3049f69696c268b537c4ae&type=all&eventType=all&minX="+minX+"&maxX="+maxX+"&minY="+minY+"&maxY="+maxY+"&getType=json");
        Seoul_Emergency(Seoul, seoul_emergency_raw);
        if(Seoul.name == "none") return Seoul.MessageStart() + "근처 돌발 상황이 없습니다.\n";
        return Seoul.MessageStart() + Seoul.tempa;
    } 

}

//////////

// 대구
std::string Daegu(std::string information, int longitude, int latitude, int angle, std::string time){
    
    // 돌발
    if(true){
        Region Daegu(longitude, latitude, angle, "돌발", time, "대구데이터허브", "#B9062F", "엔드");
        std::string daegu_emergency_raw = APIget("https://apis.data.go.kr/6270000/service/rest/dgincident?serviceKey=%2BbAZ2SEPpLhEGml%2F%2B4jAHnh4MdIWlFcmen29jq45me7i2ZEoCcOL5atnrNtt2moOqB7PXz16oZX1FIF%2BKnNKvg%3D%3D&pageNo=1&numOfRows=1000");
        Daegu_Emergency(Daegu, daegu_emergency_raw);
        if(Daegu.name == "none") return Daegu.MessageStart() + "근처 돌발 상황이 없습니다.\n";
        return Daegu.MessageStart() + Daegu.tempa;
    }

}

//////////

// 그외
std::string Else(std::string information, int longitude, int latitude, int angle, std::string time){

    Region Else(longitude, latitude, angle, information, time, "없음", "#000000", "엔드");
    return Else.MessageStart() + "서비스 제공 범위 밖입니다.\n";

}

//////////////////////////////
//////////////////////////////

// URL에서 데이터를 가져오기
std::string APIget(const std::string& url){
    
    // libcurl 초기화
    CURL* curl = curl_easy_init();

    if (curl) {
        // 요청을 저장할 문자열
        std::string api;

        // curl 옵션 설정
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &api);

        // 리다이렉션 따르기
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // 인증서 무시
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        //----//

        // 요청 수행
        CURLcode res = curl_easy_perform(curl);

        //----//

        // 결과 확인
        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            api = "curl_easy_perform() failed: ";
            return api;
        } 
        else { 
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return api;
        }  
    } 
    else {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        std::string api = "Failed to initialize libcurl";
        return api;
    }

}

// curl이 데이터를 받을 때의 함수
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}