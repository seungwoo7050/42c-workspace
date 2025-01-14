# c-foundation

![Language](https://img.shields.io/badge/language-C99-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)

42 `libft` 과제를 변형한 C 프로젝트입니다. 문자 검사, 메모리, 문자열, fd 출력, 단일 연결 리스트 함수 43개를 정적 라이브러리로 묶습니다.

## 빌드

```sh
make        # 라이브러리 빌드
make clean  # build/ 삭제
make re     # 처음부터 다시 빌드
```

산출물은 `build/libft.a`, 공개 헤더는 `include/libft.h`입니다.

## 사용

```c
#include "libft.h"
#include <stdlib.h>

int	main(void)
{
	t_list	*list;
	char	*text;

	text = ft_strjoin("hello, ", "libft");
	if (text == NULL)
		return (1);
	list = ft_lstnew(text);
	if (list == NULL)
		return (free(text), 1);
	ft_putendl_fd(list->content, 1);
	ft_lstclear(&list, free);
	return (0);
}
```

```sh
cc -std=c99 -Wall -Wextra -Werror -Iinclude example.c build/libft.a -o example
```

## 테스트

```sh
make test
```

`make test-asan`은 같은 테스트를 AddressSanitizer와 UndefinedBehaviorSanitizer로 실행합니다. 실패하면 0이 아닌 종료 코드로 끝납니다.

## 범위와 제한

- 바이트와 NUL 종료 문자열만 다룹니다. 문자 함수는 ASCII 범위만 판정하고 locale은 보지 않습니다.
- `ft_strlen` 같은 기본 함수는 NULL 인자를 검사하지 않습니다. 새 문자열을 만드는 함수(`ft_substr`, `ft_strjoin`, `ft_strtrim`, `ft_split` 등)는 NULL 인자나 할당 실패에 NULL을 돌려줍니다.
- 리스트는 `t_list` 단일 연결 리스트만 제공합니다.
- `fclean` 타깃은 없습니다. `make clean`이 라이브러리까지 지웁니다.
