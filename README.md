# 🐾 WATCH-PET| 터미널 상주형 코딩 메이트 서비스

# 1. 소개

다소 단조롭고 삭막하던 CLI 코딩 환경에 시각적인 재미를 주고, 랭킹 시스템과 채팅 기능을 통해 다른 사용자들과 자극을 주고 받으면서 학생들에게 지속적인 학습 동기를 부여하고자 본 프로젝트를 진행하였다. WATCH-PET은 사용자의 코딩 작업을 감시(Watch)한다는 기술적 의미와, 학습 시간의 척도가 되는 시계(Watch)의 의미를 동시에 담고 있다.


# 2. 주요 기능 목록

### 1) 사용자 로그인 및 데이터 관리 기능

- **기능 설명:** 사용자가 입력한 username을 고유 계정 ID처럼 활용하여 프로그램이 종료되더라도 펫의 성장 상태가 유지되도록 설계했다. 커널에서 정확한 시각을 받아와 접속 기록을 누적 저장하며, 사용자는 대시보드 창에서 `/history` 명령어를 통해 과거 접속 기록을 언제든지 확인할 수 있다.
- **관련 시스템 콜 및 기술:**
    - `open`, `read`, `write` 시스템 콜 기반의 저수준 파일 입출력을 통한 레벨/경험치 데이터 영속성 확보
    - `time` 시스템 콜 및 `ctime` 함수를 활용한 로그인 타임스탬프 기록

### 2) 실시간 코딩 프로세스 모니터링 기능

- **기능 설명:** 제안서에서 계획했던 alarm 타이머 방식 대신 Named Pipe(명명된 파이프) 방식을 새롭게 채택했다. 워크스페이스의 타자 입력 이벤트를 대시보드로 즉시 전송하여, 키보드를 칠 때마다 실시간으로 펫의 경험치가 오르는 프로세스 간 통신(IPC)을 구현했다.
- **관련 시스템 콜 및 기술:**
    - `mkfifo` 시스템 콜을 호출하여 두 프로세스를 연결하는 파이프 생성
    - `open`, `read`, `write` 시스템 콜을 활용한 파이프 기반 실시간 데이터 송수신

### 3) 실시간 작업 파일 저장 기능

- **기능 설명:** 사용자가 코딩창에 타자를 치는 즉시 디스크 파일에 데이터가 안전하게 저장된다. 특히 백스페이스 처리 시 터미널 화면의 글자만 지우는 것이 아니라, 실제 파일의 마지막 바이트를 물리적으로 즉시 잘라내어 정합성을 유지하도록 구현했다.
- **관련 시스템 콜 및 기술:**
    - `lseek` 및 `ftruncate` 시스템 콜을 사용한 물리적 파일 오프셋 및 크기 제어

### 4) 터미널 기반 유저 간 실시간 채팅 기능

- **기능 설명:** 다중 접속 네트워크 환경을 구축하여 전 세계 유저와 실시간 채팅이 가능하다. 서버 통신 병목 현상을 방지하기 위해 유저마다 독립된 스레드를 할당해 동시 처리 성능을 높였으며, 다수가 동시에 메시지를 보낼 때 발생할 수 있는 경쟁 상태(Race Condition)를 완벽히 차단했다.
- **관련 시스템 콜 및 기술:**
    - `socket` 관련 시스템 콜을 활용한 다중 접속 네트워크 통신 구축
    - `pthread_create` 멀티스레딩 및 `mutex`를 활용한 공유 자원 동기화 제어

### 5) 랭킹 산출 및 실시간 브로드캐스트 기능

- **기능 설명:** 다중 접속망 위에서 서버는 각 클라이언트가 보내오는 타건 및 경험치 데이터를 지속적으로 수신한다. 수집된 데이터는 내부 정렬 알고리즘을 거쳐 상위 5위까지의 글로벌 랭킹으로 산출되며, 갱신된 데이터는 현재 접속해 있는 모든 유저의 대시보드로 즉각 브로드캐스트된다.
- **관련 시스템 콜 및 기술:**
    - `read` 시스템 콜을 지속적으로 호출하여 실시간 데이터 수신
    - `write` 시스템 콜을 활용한 랭킹 및 채팅 갱신 데이터 브로드캐스트


# 3. 스크린샷 (또는 데모)

- 전체 실행 화면(좌측: 대시보드, 우측상단: 서버, 우측하단:워크스페이스)

<img width="2880" height="1800" alt="image" src="https://github.com/user-attachments/assets/ca833aaa-c5c9-4e3c-9559-477af03ae89d" />



- 실행영상 1-로그인 유지 및 파일 작업과 저장

[https://drive.google.com/file/d/161oQebjzEnKad-VXFv-bd2ibX524rb9m/view?usp=drive_link](https://drive.google.com/file/d/161oQebjzEnKad-VXFv-bd2ibX524rb9m/view?usp=drive_link)


- 실행영상 2-실시간 랭킹 반영

[https://drive.google.com/file/d/13kUdKEGgl3htDtMucSsZYvvSugB0SGq-/view?usp=drive_link](https://drive.google.com/file/d/13kUdKEGgl3htDtMucSsZYvvSugB0SGq-/view?usp=drive_link)


- 실행영상 3-로그인 기록 관리

[https://drive.google.com/file/d/1aGF_npw3lCUmoVAjO19f_uMtZWA3fwOr/view?usp=drive_link](https://drive.google.com/file/d/1aGF_npw3lCUmoVAjO19f_uMtZWA3fwOr/view?usp=drive_link)


- 실행영상 4- 사용자간 채팅

[https://drive.google.com/file/d/1XttCq6CSHs0aO35IeJ70kGJGNDBX6iwh/view?usp=drive_link](https://drive.google.com/file/d/1XttCq6CSHs0aO35IeJ70kGJGNDBX6iwh/view?usp=drive_link)


# 4. 빌드 및 실행 방법

**[환경 준비]**

- OS: Linux 환경 권장
- 필요 라이브러리: `ncurses`, `pthread`

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

**[빌드 방법]**
제공된 Makefile을 사용하여 터미널에 아래 명령어를 입력하면 전체 모듈이 빌드됩니다.

```bash
make
```

**[실행 방법]**
빌드가 완료되면, 다음 3개의 프로그램을 각각 **독립된 터미널 창**에서 순서대로 실행합니다.

**-글로벌 서버 실행**

```bash
./watchpet_server
```

**-대시보드 접속**

```bash
./watchpet_dash
# 서버 IP (로컬인 경우 127.0.0.1) 및 자신의 닉네임 입력
```

**-코딩 워크스페이스 실행**

```bash
./watchpet_workspace
# 연동할 대시보드의 닉네임과 편집할 파일명(예: main.c) 입력 후 코딩 시작
# 종료 시 '#' 입력
```


# 5. 팀원 정보
**[Team 7]**

<img width="547" height="244" alt="image" src="https://github.com/user-attachments/assets/ad23fc74-a351-4090-a4e7-a426f25142a0" />


