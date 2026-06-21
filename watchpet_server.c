#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10

typedef struct {
    int sock;
    char name[50];
    int level;
    int exp;
    int active;
} Client;

Client clients[MAX_CLIENTS];
char chat_history[10][128];
int chat_count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// 랭킹 정렬용 비교 함수 (레벨 내림차순 -> 경험치 내림차순)
int compare_clients(const void* a, const void* b) {
    Client* c1 = (Client*)a;
    Client* c2 = (Client*)b;
    if (c1->level != c2->level) return c2->level - c1->level;
    return c2->exp - c1->exp;
}

void broadcast_chat() {
    char packet[2048] = "CHAT:";
    for (int i = 0; i < 10; i++) {
        if (strlen(chat_history[i]) > 0) {
            strcat(packet, chat_history[i]);
            strcat(packet, "\n");
        }
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) write(clients[i].sock, packet, strlen(packet));
    }
}

void broadcast_ranking() {
    Client temp[MAX_CLIENTS];
    int count = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strlen(clients[i].name) > 0) temp[count++] = clients[i];
    }
    qsort(temp, count, sizeof(Client), compare_clients);

    char packet[1024] = "RANK:";
    for (int i = 0; i < count && i < 5; i++) { // 상위 5명
        char line[128];
        sprintf(line, "%d. %s (Lv.%d, %d%%)\n", i + 1, temp[i].name, temp[i].level, temp[i].exp / 10);
        strcat(packet, line);
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) write(clients[i].sock, packet, strlen(packet));
    }
}

void* handle_client(void* arg) {
    int idx = *(int*)arg;
    int sock = clients[idx].sock;
    char buffer[256];

    // 접속 시 기존 10줄 채팅 내역 즉시 전송
    pthread_mutex_lock(&lock);
    broadcast_chat();
    broadcast_ranking();
    pthread_mutex_unlock(&lock);

    while (read(sock, buffer, sizeof(buffer) - 1) > 0) {
        buffer[255] = '\0';
        pthread_mutex_lock(&lock);
        
        if (strncmp(buffer, "STAT:", 5) == 0) {
            // 경험치 업데이트 수신: STAT:이름 레벨 경험치
            sscanf(buffer + 5, "%s %d %d", clients[idx].name, &clients[idx].level, &clients[idx].exp);
            broadcast_ranking();
        } 
        else if (strncmp(buffer, "MSG:", 4) == 0) {
            // 채팅 메시지 수신 (10줄 원형 큐 관리)
            for (int i = 0; i < 9; i++) strcpy(chat_history[i], chat_history[i + 1]);
            strncpy(chat_history[9], buffer + 4, 127);
            broadcast_chat();
        }
        memset(buffer, 0, sizeof(buffer));
        pthread_mutex_unlock(&lock);
    }

    // 클라이언트 종료 처리
    pthread_mutex_lock(&lock);
    clients[idx].active = 0;
    broadcast_ranking();
    pthread_mutex_unlock(&lock);
    close(sock);
    return NULL;
}

int main() {
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);
    printf("WATCH-PET 글로벌 랭킹/채팅 서버 시작됨 (Port: %d)...\n", PORT);

    for (int i = 0; i < 10; i++) chat_history[i][0] = '\0';

    while (1) {
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].active) {
                clients[i].sock = client_sock;
                clients[i].active = 1;
                pthread_t tid;
                int* idx = malloc(sizeof(int)); *idx = i;
                pthread_create(&tid, NULL, handle_client, idx);
                break;
            }
        }
        pthread_mutex_unlock(&lock);
    }
    close(server_sock);
    return 0;
}
