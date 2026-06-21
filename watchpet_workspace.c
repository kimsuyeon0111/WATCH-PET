#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

int main() {
    char target_user[50];
    char filename[50];
    
    printf("연동할 대시보드 사용자(username): ");
    scanf("%49s", target_user);
    
    printf("열거나 생성할 파일명 (예: main.c): ");
    scanf("%49s", filename);

    char target_fifo[128];
    sprintf(target_fifo, "/tmp/watch_pet_exp_%s", target_user);

    // [조건 5] O_TRUNC(덮어쓰기)를 뺐으므로 기존 파일이 유지됨
    int file_fd = open(filename, O_RDWR | O_CREAT, 0644);
    int fifo_fd = open(target_fifo, O_WRONLY | O_NONBLOCK);

    if (fifo_fd == -1) {
        printf("오류: '%s' 대시보드가 실행되어 있지 않습니다!\n", target_user);
        return 1;
    }

    // [조건 5-1] 기존 파일이 있다면 그 내용을 화면에 그대로 출력
    printf("\n=== WATCH-PET 코딩 워크스페이스 ===\n");
    char c;
    while (read(file_fd, &c, 1) > 0) {
        write(STDOUT_FILENO, &c, 1);
    }
    // 이어서 작성할 수 있도록 파일 포인터를 맨 끝으로 이동
    lseek(file_fd, 0, SEEK_END);

    printf("\n\n--- [ 이 위쪽은 기존 내용입니다. 이어서 코딩하세요 ('#' 종료) ] ---\n");

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;
    while (read(STDIN_FILENO, &ch, 1) > 0) {
        if (ch == '#') break; 

        // 백스페이스 물리적 파일 삭제 처리
        if (ch == 127 || ch == '\b') {
            off_t current_pos = lseek(file_fd, 0, SEEK_CUR);
            if (current_pos > 0) {
                write(STDOUT_FILENO, "\b \b", 3);
                ftruncate(file_fd, current_pos - 1);
                lseek(file_fd, current_pos - 1, SEEK_SET);
            }
            continue; 
        }

        write(STDOUT_FILENO, &ch, 1);
        
        if (ch == '\r') {
            char newline = '\n';
            write(file_fd, &newline, 1);
            write(STDOUT_FILENO, &newline, 1);
        } else {
            write(file_fd, &ch, 1);
        }

        // [조건 2-1] 타자 1번 칠 때마다 1바이트 신호 전송 (대시보드에서 1포인트, 즉 0.1%로 환산)
        write(fifo_fd, "1", 1);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    close(file_fd);
    close(fifo_fd);
    printf("\n워크스페이스 종료. 파일이 안전하게 디스크에 저장되었습니다.\n");

    return 0;
}
