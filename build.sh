gcc -std=c23 \
  -g3 -Og -fno-omit-frame-pointer \
  -Wall -Wextra -Wformat=2 -Werror=format-security \
  -Wshadow -Warray-bounds=2 -Wstrict-overflow=5 -Wconversion -Wcast-align \
  -Wnull-dereference -Wvla \
  -fsanitize=address,undefined -fsanitize-address-use-after-scope \
  -fstack-protector-strong -D_FORTIFY_SOURCE=3 \
  main.c libs/*.c controller/*.c state.c -o main
