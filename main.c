// 헤더 정의
#include "header/init.h"
#include "header/structure.h"
#include "header/functions.h"
#include "header/defines.h"

// 메인 함수 시작 => 메인 함수에서는 추후 프로그램의 크기가 커질 것을 감안하여 전체적인 프로그램의 진행에 대해서만 알 수 있도록 구성하였음.
int main() {
  
  // 변수 선언
  char env_file[FILE_NAME_SIZE]; // 환경 변수가 기록되어 있는 파일의 이름이 저장될 변수
  char log_file[ENV_VALUE_SIZE]; // 로그가 기록되어 있는 파일(writelog.txt)의 이름이 저장될 변수
  char log_path[PATH_SIZE]; // 로그가 기록되어 있는 파일(writelog.txt)의 디렉터리(path)가 저장될 변수

  // 환경 변수 파일(setup.txt)의 데이터로 "read할 로그 파일과 패스"를 가져온다.
  get_log_path(env_file, log_file, log_path);
  // 로그 파일의 "로그 데이터를 거꾸로 출력"해준다.
  print_log(env_file, log_file, log_path);

  // 프로그램 종료
  exit(0);
} // 메인 함수 끝

