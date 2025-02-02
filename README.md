# format-printer

![Language](https://img.shields.io/badge/language-C99-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)

42 `ft_printf` 과제를 변형한 C 프로젝트입니다. 포맷 문자열을 해석해 fd 1에 쓰는 `ft_printf`를 정적 라이브러리로 제공합니다.

## 빌드

```sh
make        # 라이브러리 빌드
make clean  # build/ 삭제
make re     # 처음부터 다시 빌드
```

산출물은 `build/libftprintf.a`, 공개 헤더는 `include/ft_printf.h`입니다.

## 사용

```c
#include "ft_printf.h"

int	main(void)
{
	int	written;

	written = ft_printf("[%-6s|%05d|%#x|%p]\n", "abc", -42, 255u, (void *)0);
	return (written < 0);
}
```

```sh
cc -std=c99 -Wall -Wextra -Werror -Iinclude example.c build/libftprintf.a -o example
```

변환은 `%c %s %p %d %i %u %x %X %%`, 플래그는 `-`, `0`, `#`, 공백, `+`, 그리고 십진 너비와 `.` 정밀도를 지원합니다. 성공하면 쓴 바이트 수를, 지원하지 않는 형식이나 쓰기 오류면 -1을 돌려줍니다.

## 테스트

```sh
make test
```

일반 출력 테스트와 `write` 오류 주입 테스트(짧은 쓰기, `EINTR`, `EPIPE`, 0바이트 반환)를 함께 실행합니다.

## 범위와 제한

- 길이 지정자(`l`, `h` 등), `*` 너비·정밀도, 위치 인자, 부동소수점 변환은 지원하지 않습니다. 이런 형식이 있으면 아무것도 쓰지 않고 -1을 돌려줍니다.
- 출력 fd는 1로 고정입니다.
- 쓰는 도중 오류가 나면 이미 쓴 바이트는 되돌리지 않습니다. `EINTR`만 재시도하고, 비차단 fd의 `EAGAIN`은 오류로 처리합니다.
- `SIGPIPE` 처리 방식은 바꾸지 않습니다.
