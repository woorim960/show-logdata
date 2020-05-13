// functions/print_log.c

// 헤더 정의
#include "../header/init.h"
#include "../header/structure.h"
#include "../header/defines.h"

// 함수 선언
void show_env(char env_file[]);

// 로그를 출력할 메인 함수 =>  매개변수 (환경 변수 파일, 로그 파일, 로그 패스)
void print_log(char env_file[], char log_file[], char log_path[]) {

  // writelog.txt의 레코드를 원활하게 사용하도록 도와줄 레코드 선언
  RECEIVE_RECORD log_data;

  // 변수 선언
  int log_fd;
  int read_len = 1; // read 데이터의 길이를 저장할 변수
  int log_size = 0; // 로그 데이터의 레코드 갯수를 카운팅할 변수
  int start_seq = 1; // 터미널의 출력될 로그 데이터의 시작 시퀀스 값
  int selector; // 5: 재조회 7:앞으로 8:뒤로 9:종료 의 상태에 따라 출력 로그를 변경시켜줄 선택자 변수

  // set_env에서 설정된 로그 파일을 open
  if ((log_fd = open(log_path, O_RDONLY)) == -1) {
    printf("    %s : 해당 경로 혹은 파일이 존재하지 않습니다. (에러코드 : 1)\n", log_path);
    exit(1);
  }

  // 파일의 전체 레코드 갯수를 세기 위한 반복문
  while (read_len) {
    // 커서(lseek)를 이동하여 모든 레코드를 읽고, 레코드 갯수 카운팅
    lseek(log_fd, log_size * sizeof(log_data), SEEK_SET);
    read_len = read(log_fd, &log_data, sizeof(log_data));
    log_size++; // 레코드 갯수 카운팅
  }

  // 무한 반복
  while (1) {
    // 터미널을 꺠끗히 비운다.
    system("clear");

    // UI 출력
    puts("\n    ============ 소켓 통신 로그 ============\n");

    // 시작 시퀀스 값부터 10개까지 출력한다.
    for (int i = start_seq; i < (start_seq+10); i++) {
      // SEEK_END로 맨 뒤부터 데이터를 읽게 된다. => 즉, 거꾸로 출력해준다.
      lseek(log_fd, -i * sizeof(log_data), SEEK_END);
      read(log_fd, &log_data, sizeof(log_data));
      // 데이터 출력
      printf("    %s %s\n\n", log_data.datetime, log_data.message);
    }

    // UI 출력
    puts("    ========================================");
    puts("    5: 재조회 7:앞으로 8:뒤로 9:종료 0:환경변수조회");
    printf("    >> ");
    scanf("%d", &selector);

    // 사용자의 입력 값대로 분기하여 실행
    switch(selector) {
      case 5:
        // 재조회 입력시 시작 시퀀스를 1로 초기화
        start_seq = 1;
        break;
      case 7:
        // 앞으로 입력시 "삼항 연산자를 이용"하여 시작 시퀀스가 1보다 작거나 같으면 1로 초기화하여 더이상 앞으로 이동하지 못하도록하고, 1보다 크다면(11, 21, 31 ... 등의 경우) 시작 시퀀스 값을 -10하여 앞 10개의 레코드를 조회하도록 한다.
        start_seq = (start_seq <= 1) ? 1 : start_seq - 10;
        break;
      case 8:
        // 뒤로 입력시 "삼항 연산자를 이용"하여 시작 시퀀스가 전체 로그 갯수보다 크다면 현재 시퀀스를 그대로두어 더이상 뒤로 이동하지 못하도록하고, 전체 로그 갯수보다 작다면(1, 11, 21, 31 ... Max Log Size) 시작 시퀀스 값을 +10하여 뒤 10개의 레코드를 조회하도록 한다.
        start_seq = (start_seq+10 > log_size) ? start_seq : start_seq + 10;
        break;
      case 9:
        // 종료 입력시 프로그램 종료
        puts("\n    프로그램을 종료합니다.\n");
        close(log_fd);
        exit(0);
        break;
      case 0:
        // "환경 변수 보기" 입력시 환경 변수를 조회할 함수 실행
        show_env(env_file);
        break;
      default:
        break;
    }
  }

  // 파일 디스크립터 반환(종료)
  close(log_fd);
}

// 환경 변수를 조회해줄 함수 =>  매개변수 (환경변수 파일)
void show_env(char env_file[]) {

  // 환경 변수를 조회하기 위해 필요한 구조체 선언
  ENV_PARAMETER env;

  // 변수 선언
  int env_fd;
  int end_seq; // 환경변수의 마지막 시퀀스가 담길 변수
  int selector; // 상태에 따라 분기할 선택자 변수

  // 터미널을 꺠끗히 비운다.
  system("clear");

  // 환경 변수 셋업 파일 오픈
  if ((env_fd = open(env_file, O_RDONLY)) == -1) {
    puts("    파일이 존재하지 않거나 잘못된 파일명입니다. (에러코드 : 1)");
    exit(1);
  }

  // UI 출력문
  puts("");
  puts("    =================== 환경 변수 조회 ===================\n");
  puts("    Seq    Parameter            Parameter value");
  puts("    -----  -----------------    -----------------------");

  // 마지막 시퀀스 찾기
  lseek(env_fd, -sizeof(env), SEEK_END);
  read(env_fd, &env, sizeof(env));
  end_seq = env.seq;

  // 환경 변수의 모든 데이터를 조회
  for (int i = 0; i < end_seq; i++) {
    // 커서(lseek)를 이동하여 모든 레코드를 조회
    lseek(env_fd, i * sizeof(env), SEEK_SET);
    read(env_fd, &env, sizeof(env)); // 읽은 데이터가 0이면(없으면) 반복문 종료

    // 환경변수 출력
    printf("    %-6d %-20s %s\n", env.seq, env.key, env.value);
  }

  // 파일디스크립터 반환
  close(env_fd);

  // UI 출력문
  puts("\n    =====================================");
  puts("    5: 로그조회 9:종료");

  // selector가 5면 반복문 종료(로그조회로 이동됨)
  while(selector != 5) {
    printf("    >> ");

    scanf("%d", &selector);
    if (selector == 9) {
      puts("\n    프로그램을 종료합니다.\n");
      exit(0);
    }
  }
}