// 라이브러리
#include <iostream>
#include <thread> // 멀티쓰레드
#include <cstdlib> // 에러메세지 출력 및 프로그램 종료
#include <cstring> // 메모리 초기화
#include <netinet/in.h> // 네트워크 프로그래밍 
#include <unistd.h> // close 함수

// 헤더파일
#include "CtoR.h" // Reader로 연계

// 원형
void Thread(int clientSocket); // 클라이언트 쓰레드 함수
std::string WordExtract(std::string receivedData, std::string frontword){
    size_t startPos = receivedData.find(frontword, 0) + frontword.size();
    size_t endPos = receivedData.find("\n", startPos);
    return receivedData.substr(startPos, endPos - startPos);
}

//////////////////////////////
//////////////////////////////

int main() {

    // 서버소켓 제작
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성
    if (serverSocket == -1) {
        std::cerr << "소켓 생성 실패" << std::endl;
        return 1;
    }

    // 서버소켓특성 제작
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; // 소켓특성
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    int optval = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        std::cerr << "setsockopt 실패" << std::endl;
        close(serverSocket);
        return 1;
    }
    const int PORT = 8080; // 포트넘버
    serverAddress.sin_port = htons(PORT);

    // 서버소켓에 특성 바인딩
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "바인딩 실패" << std::endl;
        close(serverSocket);
        return 1;
    }

    // 서버소켓 대기모드 전환
    if (listen(serverSocket, 10) == -1) {
        std::cerr << "대기 실패" << std::endl;
        close(serverSocket);
        return 1;
    }
    std::cout << "서버가 " << PORT << " 포트에서 대기 중..." << std::endl;

    //////////////////////////////

    while (true) {
        
        // 클라이언트 접근 대기 및 수락
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            std::cerr << "연결 수락 실패" << std::endl;
            continue;
        }
        
        // 클라이언트의 쓰레드로 들어간다
        std::thread clientThread(Thread, clientSocket);
        clientThread.detach();
        
    }

    //////////////////////////////

    // 서버 소켓 닫기
    close(serverSocket);
    return 0;

}

//////////////////////////////
//////////////////////////////

// Thread
void Thread(int clientSocket) {
    
    // 클라이언트로부터 데이터 받기
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "recv 실패" << std::endl;
        close(clientSocket);
        return;
    }

    // 버퍼에서 실데이터 추출
    std::string receivedData(buffer);
    size_t headerEnd = receivedData.find("\r\n\r\n"); // 앞쪽제거
    if (headerEnd != std::string::npos) {
        receivedData = receivedData.substr(headerEnd + 4);
    }
    else { 
        std::cerr << "유효한 HTTP 형식이 아닙니다." << std::endl;
        close(clientSocket);
        return;
    }
    size_t lastNonSpace = receivedData.find_last_not_of(" \t\r\n"); // 뒤쪽제거
    if (lastNonSpace != std::string::npos) {
        receivedData = receivedData.substr(0, lastNonSpace + 1);
    }
    else { 
        std::cerr << "유효한 HTTP 형식이 아닙니다." << std::endl;
        close(clientSocket);
        return;
    }

    // 정보, 경도, 위도, 각도를 추출해서 변수에 담기
    if(receivedData.find("정보 : ", 0) == std::string::npos) {
        std::cerr << "유효한 HTTP 형식이 아닙니다." << std::endl;
        close(clientSocket);
        return;
    }
    std::string information = WordExtract(receivedData, "정보 : ");
    int longitude = std::stoi(WordExtract(receivedData, "경도 : "));
    int latitude = std::stoi(WordExtract(receivedData, "위도 : "));
    int angle = std::stoi(WordExtract(receivedData, "각도 : "));
    std::string time = WordExtract(receivedData, "시간 : ");

    //////////////////////////////

    // 응답메세지의 엔티티바디 제작
    std::string entitybody = "";

    // 엔티티바디로 결과정보 가져오기
    entitybody += Selector(information, longitude, latitude, angle, time);

    // 응답메세지의 헤더 제작
    std::string header;
    
    // 헤더에 정보 추가
    header = "HTTP/1.1 200 OK\r\n";
    header += "Content-Type: text/plain\r\n";
    header += "Content-Length: " + std::to_string(entitybody.size()) + "\r\n";
    header += "Access-Control-Allow-Origin: *\r\n";
    header += "\r\n";

    //////////////////////////////

    // 헤더 전송
    ssize_t bytesSent;
    bytesSent = send(clientSocket, header.c_str(), header.size(), 0);
    if (bytesSent == -1) {
        std::cerr << "헤더 전송 실패" << std::endl;
        close(clientSocket);
        return;
    }

    // 엔티티바디 전송
    bytesSent = send(clientSocket, entitybody.c_str(), entitybody.size(), 0);
    if (bytesSent == -1) {
        std::cerr << "헤더 전송 실패" << std::endl;
        close(clientSocket);
        return;
    }

    // 클라이언트소켓 닫기
    close(clientSocket);
    return;

}