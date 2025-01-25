# buffered-line-reader

![Language](https://img.shields.io/badge/language-C99-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)

42 `get_next_line` 과제를 변형한 C 프로젝트입니다. fd에서 한 줄씩 읽는 `get_next_line`과, 읽기 상태를 호출자가 쥐는 컨텍스트 API(`blr_reader_*`)를 정적 라이브러리로 제공합니다.

## 빌드

```sh
make        # 라이브러리 빌드
make clean  # build/ 삭제
make re     # 처음부터 다시 빌드
```

산출물은 `build/libbuffered_line_reader.a`, 공개 헤더는 `include/get_next_line.h`입니다. read 버퍼 크기는 기본 42이며 `make re BUFFER_SIZE=1024`처럼 바꿉니다.

## 사용

```c
#include "get_next_line.h"
#include <stdio.h>
#include <stdlib.h>

int	main(void)
{
	t_blr_reader	*reader;
	t_blr_result	result;
	char			*line;

	reader = blr_reader_create(0);
	if (reader == NULL)
		return (1);
	while ((result = blr_reader_next(reader, &line)) == BLR_LINE)
	{
		fputs(line, stdout);
		free(line);
	}
	blr_reader_destroy(reader);
	return (result == BLR_ERROR);
}
```

```sh
cc -std=c99 -Wall -Wextra -Werror -Iinclude example.c build/libbuffered_line_reader.a -o example
```

`blr_reader_next`는 `BLR_LINE`(줄 끝 `\n` 포함, `*line`은 호출자가 해제), `BLR_EOF`, `BLR_AGAIN`(비차단 fd에 아직 입력 없음), `BLR_ERROR`를 돌려줍니다. `get_next_line(fd)`는 다음 줄 또는 NULL을 돌려줍니다.

## 테스트

```sh
make test
```

`make test-asan`은 같은 테스트를 AddressSanitizer와 UndefinedBehaviorSanitizer로 실행합니다. 두 타깃 모두 `make test BUFFER_SIZE=1`처럼 버퍼 크기를 바꿔 돌릴 수 있습니다.

## 범위와 제한

- 컨텍스트는 fd를 빌릴 뿐 닫지 않습니다. 외부에서 `lseek`로 위치를 바꿨다면 `blr_reader_reset`을 호출해야 합니다.
- `dup`으로 같은 open file description을 공유하는 fd에는 컨텍스트를 하나만 둬야 합니다.
- 한 컨텍스트를 여러 스레드에서 동시에 호출할 수 없습니다. `get_next_line`은 fd별 상태를 전역 리스트에 두므로 스레드 안전하지 않습니다.
- 줄을 NUL 종료 문자열로 돌려주므로 중간에 NUL 바이트가 있는 입력은 길이를 표현하지 못합니다.
