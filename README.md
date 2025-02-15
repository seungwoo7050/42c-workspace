# signal-message-bus

![Language](https://img.shields.io/badge/language-C-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)

42 `minitalk` 과제를 변형한 C 프로젝트입니다. client가 문자열을 `SIGUSR1`/`SIGUSR2` 비트로 server에 보내고, UNIX domain datagram 소켓 응답 채널로 세션 수락과 비트마다의 수신 확인을 받습니다.

## 빌드

```sh
make        # server, client 빌드
make clean  # build/ 삭제
make re     # 처음부터 다시 빌드
```

산출물은 `build/bin/server`, `build/bin/client`입니다.

## 실행

```sh
./build/bin/server                                 # 터미널 1: PID를 출력하고 대기
./build/bin/client <server_pid> "hello, signal bus"  # 터미널 2
```

`client`는 server PID와 메시지 두 인자를 받습니다. server는 받은 메시지를 한 줄로 출력합니다. PID에 server가 없거나, server가 다른 client와 세션 중이거나, 응답이 비트마다 3초 안에 오지 않으면 `client`는 표준 오류에 원인을 쓰고 종료 코드 1로 끝납니다.

## 테스트

```sh
make test
```

PID 파싱, 기본 송수신, 세션 소유권, 응답 출처 검증, 출력 실패, stale 소켓, 높은 fd 번호, 상속된 signal mask를 셸 스크립트로 검사합니다.

## 범위와 제한

- server 하나는 한 번에 client 하나만 받습니다.
- 응답 소켓은 `/tmp/signal-message-bus-<uid>/` 아래에 만들므로 server와 client는 같은 사용자로 실행해야 합니다.
- 비트 하나마다 응답을 기다리므로 메시지 길이에 비례해 느립니다.
- `-std` 옵션 없이 `_POSIX_C_SOURCE 200809L`로 빌드합니다.
