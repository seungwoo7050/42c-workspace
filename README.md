# 42c-workspace

![Language](https://img.shields.io/badge/language-C-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)
![Build](https://img.shields.io/badge/build-Make-lightgrey)

42 공통 과정의 C 과제 7개를 변형해 다시 구현한 프로젝트를 모은 저장소입니다. 프로젝트마다 공통 조상이 없는 독립 브랜치를 쓰고, 이 브랜치(`main`)에는 이 문서만 있습니다.

## 프로젝트

| 프로젝트 | 원 과제 | 설명 |
| --- | --- | --- |
| [c-foundation](https://github.com/seungwoo7050/42c-workspace/tree/c-foundation) | `libft` | 문자·메모리·문자열·fd 출력·연결 리스트 함수 43개를 담은 정적 라이브러리 |
| [buffered-line-reader](https://github.com/seungwoo7050/42c-workspace/tree/buffered-line-reader) | `get_next_line` | fd에서 한 줄씩 읽는 정적 라이브러리. `get_next_line`과 컨텍스트 API를 제공 |
| [format-printer](https://github.com/seungwoo7050/42c-workspace/tree/format-printer) | `ft_printf` | 9가지 변환과 플래그·너비·정밀도를 fd 1에 쓰는 `ft_printf` 정적 라이브러리 |
| [stack-sort](https://github.com/seungwoo7050/42c-workspace/tree/stack-sort) | `push_swap` | 두 스택과 11가지 연산으로 정수를 정렬하는 `push_swap`과 검증용 `checker` |
| [signal-message-bus](https://github.com/seungwoo7050/42c-workspace/tree/signal-message-bus) | `minitalk` | 문자열을 signal 비트로 보내고 UNIX domain datagram 소켓으로 비트마다 수신을 확인 |
| [thread-dining](https://github.com/seungwoo7050/42c-workspace/tree/thread-dining) | `philo` | pthread와 mutex로 구현한 식사하는 철학자 시뮬레이션 |
| [small-shell](https://github.com/seungwoo7050/42c-workspace/tree/small-shell) | `minishell` | 파이프라인·리다이렉션·heredoc·`&&`·`\|\|`를 처리하는 작은 셸 |

## 브랜치 받기

```sh
git clone https://github.com/seungwoo7050/42c-workspace.git
cd 42c-workspace
git switch small-shell                           # 브랜치 하나로 바꾸기
git worktree add ../thread-dining thread-dining  # 다른 브랜치를 옆 디렉터리에 함께 열기
```

## 공통 규약

- 공개 헤더는 `include/`, 구현은 `src/`, 테스트는 `tests/`에 두고, 산출물은 모두 `build/` 아래에 생깁니다.
- 루트 `Makefile`의 `make`, `make test`, `make clean`, `make re`로 빌드·테스트·정리합니다. `make test`는 실패하면 0이 아닌 종료 코드로 끝납니다.
- 모두 `-Wall -Wextra`로 빌드합니다. `-Werror`는 small-shell에, `-std=c99`는 signal-message-bus와 thread-dining에 없습니다.
- sanitizer 타깃은 일부 브랜치에만 있습니다. `make test-asan`은 c-foundation·buffered-line-reader·small-shell, `make test-ubsan`은 small-shell, `make test-tsan`은 thread-dining에 있습니다.
