# Просто менеджер памяти своими руками

Учебная задача. Реализация функий malloc(), calloc(), realloc и free(). Работает только под Linux. Компилировать так:

```
gcc -o memalloc.so -fPIC -shared memalloc.c
```

Для подмены системного менеджера памяти, нужно сделать так:

```
export LD_PRELOAD=$PWD/memalloc.so
```

И... Вуаля! Работает!

```
$ ls
memalloc.c		memalloc.so
```

Полный разбор, как все работает, читай [здесь](http://vdenis.ru/all/pishem-prostoy-menedzher-pamyati/)
 
