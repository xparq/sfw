::BOM...
@echo off

.\test.exe length
.\test.exe length ☺
.\test.exe length áéí
.\test.exe length áéíÓŐŰ
.\test.exe length x☺áéí
.\test.exe length árvíztúró
.\test.exe length árvíztúró☺

echo.
.\test.exe bsize
.\test.exe bsize ☺
.\test.exe bsize ☺ 0
.\test.exe bsize ☺ 1
.\test.exe bsize ☺ 999
.\test.exe bsize áéí 0
.\test.exe bsize áéí 1
.\test.exe bsize áéí 2
.\test.exe bsize áéí 3
.\test.exe bsize áéí 999
.\test.exe bsize árvíztúró
.\test.exe bsize árvíztúró☺

echo.
.\test.exe substr abcde 0
.\test.exe substr abcde 0 0
.\test.exe substr abcde 0 1
.\test.exe substr abcde 2
.\test.exe substr abcde 2 3
.\test.exe substr abcde 2 999

echo.
.\test.exe substr árvíztűrő 0
.\test.exe substr árvíztűrő 0 0
.\test.exe substr árvíztűrő 0 1
.\test.exe substr árvíztűrő 2
.\test.exe substr árvíztűrő 2 3
.\test.exe substr árvíztűrő 2 999

echo.
.\test.exe substr árvíztúró☺ 5
.\test.exe substr árvíztúró☺ 5 4
.\test.exe substr árvíztúró☺ 9 0
.\test.exe substr árvíztúró☺ 9 1
.\test.exe substr árvíztúró☺ 9 2
.\test.exe substr árvíztúró☺ 10 1
.\test.exe substr árvíztúró☺ 10 2

goto :eof
echo.
.\test.exe substr
.\test.exe substr throw
.\test.exe substr throw 999
