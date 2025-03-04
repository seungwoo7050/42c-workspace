# stack-sort

![Language](https://img.shields.io/badge/language-C99-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)

42 `push_swap` 과제를 변형한 C 프로젝트입니다. 두 스택과 11가지 연산만으로 정수를 정렬하는 명령열을 만드는 `push_swap`과, 그 명령열을 검증하는 `checker`를 빌드합니다.

## 빌드

```sh
make        # push_swap, checker 빌드
make clean  # build/ 삭제
make re     # 처음부터 다시 빌드
```

산출물은 `build/bin/push_swap`, `build/bin/checker`입니다.

## 실행

```sh
./build/bin/push_swap 3 2 1
./build/bin/push_swap "5 -1 4 0 2" | ./build/bin/checker "5 -1 4 0 2"
```

인자는 정수 여러 개 또는 공백으로 구분한 문자열 하나입니다. `push_swap`은 명령(`sa`, `pb`, `rra` 등)을 한 줄에 하나씩 출력하고, `checker`는 표준 입력의 명령을 실행한 뒤 정렬되면 `OK`, 아니면 `KO`를 출력합니다. 중복, `int` 범위 밖, 숫자가 아닌 인자나 알 수 없는 명령은 표준 오류에 `Error`를 쓰고 종료 코드 1로 끝납니다.

## 테스트

```sh
make test
```

스택 연산 invariant 테스트, 입출력 테스트, malloc/read 오류 주입 테스트를 차례로 실행합니다. 뒤의 두 단계는 `python3`가 필요합니다.

## 범위와 제한

- 입력이 5개 이하면 전용 분기로, 그보다 많으면 값을 순위로 바꾼 뒤 기수 정렬로 정렬합니다.
- 테스트가 확인하는 명령 수 상한은 무작위 100개 입력에 1500회, 500개 입력에 8000회입니다.
- 인자가 없으면 아무것도 출력하지 않고 0으로 끝납니다.
