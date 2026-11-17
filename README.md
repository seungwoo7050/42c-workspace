# small-shell

![Language](https://img.shields.io/badge/language-C99-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)

42 `minishell` 과제를 변형한 C 프로젝트입니다. 명령줄을 토큰화·파싱·확장한 뒤 리다이렉션, heredoc, 파이프라인, builtin과 외부 명령을 실행하는 작은 셸입니다.

## 빌드

```sh
make        # small-shell 빌드
make clean  # build/ 삭제
make re     # 처음부터 다시 빌드
```

산출물은 `build/bin/small-shell`입니다. readline으로 줄을 읽게 하려면 `make clean && make readline`으로 빌드합니다(`-lreadline` 필요, 기존 오브젝트가 있으면 `make readline`만으로는 다시 컴파일되지 않습니다).

## 실행

```sh
./build/bin/small-shell
```

인자는 받지 않고 표준 입력에서 한 줄씩 읽습니다. `|`, `<`, `>`, `>>`, `<<`, `&&`, `||`, `;`, 괄호 그룹 `( ... )`, 작은따옴표·큰따옴표, `$NAME`과 `$?` 확장, builtin `echo`, `pwd`, `cd`, `env`, `export`, `unset`, `exit`를 지원합니다.

## 문법과 파서

파서는 재귀 하강으로 아래 문법을 구현합니다. `&&`와 `||`는 우선순위가 같고 왼쪽부터 묶입니다. `(`를 만나면 `parse_command`가 `parse_list`를 다시 호출하므로 그룹은 중첩할 수 있고, 깊이는 64단계까지 허용합니다.

```text
list     := pipeline ( ('&&' | '||' | ';') pipeline )* ';'?
pipeline := command ( '|' command )*
command  := '(' list ')' redirection* | ( word | redirection )+
```

그룹은 포크한 자식 프로세스에서 실행합니다. 그래서 `(cd /tmp; export A=1)`의 환경 변경은 바깥 셸에 남지 않고, `(exit 3)`은 셸을 끝내지 않고 종료 상태 3만 돌려줍니다. 그룹 안의 확장(`$?` 등)과 heredoc 본문 읽기는 각각 실행 시점과 줄 처리 시작 시점에 이루어집니다.

## 테스트

```sh
make test
```

`make test-asan`과 `make test-ubsan`은 같은 테스트를 각 sanitizer로 실행합니다. `make test-sanitizers-container`는 Docker의 `gcc:13-bookworm` 이미지에서 두 타깃을 실행합니다.

## 범위와 제한

- 백그라운드 실행 `&`는 문법 오류로 거부합니다. glob, job control은 구현하지 않았습니다.
- 시그널 핸들러를 두지 않으므로 Ctrl-C를 누르면 셸 자체가 기본 동작대로 종료됩니다.
- `-Werror` 없이 빌드하므로 경고가 있어도 빌드는 실패하지 않습니다.
