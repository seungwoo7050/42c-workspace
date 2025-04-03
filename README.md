# thread-dining

![Language](https://img.shields.io/badge/language-C-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)

42 `philo` 과제를 변형한 C 프로젝트입니다. 철학자마다 pthread 하나, 포크마다 mutex 하나를 두고 식사하는 철학자 문제를 시뮬레이션합니다.

## 빌드

```sh
make        # philo 빌드
make clean  # build/ 삭제
make re     # 처음부터 다시 빌드
```

산출물은 `build/bin/philo`입니다.

## 실행

```sh
./build/bin/philo 5 800 200 200      # 철학자 수, time_to_die, time_to_eat, time_to_sleep
./build/bin/philo 5 800 200 200 3    # 다섯째 인자: 철학자마다 채울 식사 횟수
```

시간 단위는 밀리초입니다. 상태가 바뀔 때마다 `<경과 ms> <번호> <상태>`를 한 줄씩 출력하고, 누군가 죽거나 모두 식사 횟수를 채우면 끝납니다. 인자 개수나 값이 맞지 않으면 사용법을 출력하고 종료 코드 1로 끝납니다.

## 테스트

```sh
make test
```

`make test-tsan`은 ThreadSanitizer로 다시 검사합니다. TSan을 쓸 수 없는 환경에서는 `tsan: skipped`를 출력하고 종료 코드 77로 끝나며, `TSAN_REQUIRED=1`을 주면 1로 끝납니다.

## 범위와 제한

- 철학자 수는 1~200, 시간과 식사 횟수는 1 이상 `INT_MAX` 이하의 정수만 받습니다.
- bonus 구현은 없습니다. `make bonus`는 메시지를 출력하고 실패합니다.
- 사망은 감시 스레드가 0.5ms 간격으로 폴링해 판정하므로 `time_to_die`보다 조금 늦게 출력될 수 있습니다.
