# 베이스 이미지 설정
FROM ubuntu:latest

# 작업 디렉토리 설정
WORKDIR /app

# 호스트 파일 시스템에서 현재 디렉토리의 모든 파일을 컨테이너의 /app 디렉토리로 복사
COPY . .

# 라이브러리 curl 설치
RUN apt-get update && apt-get install -y libcurl4-openssl-dev

# 컨테이너 실행 시 실행할 명령어
CMD ["./Server"]
