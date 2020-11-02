[![Codacy Badge](https://app.codacy.com/project/badge/Grade/c75c8247a7cd43ed8b432bb4646592ec)](https://www.codacy.com/gh/reficul0/InfinityMatrix/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=reficul0/InfinityMatrix&amp;utm_campaign=Badge_Grade)
[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](https://github.com/RocketChat/Rocket.Chat/raw/master/LICENSE)

N-мерная разряженная "бесконечная" матрица.
Матрица хранит только те элементы, значения которых задал пользователь. 
Имеется поддержка обхода по всем занятым ячейкам. При этом сообщается и значение ячейки и её позиция. 

При запросе элемента из свободной ячейки возвращается "пустое" значение.
Возвращаемое матрицей значение можно проверить на пустоту.

Каждое измерение матрицы знает сколько в нём элементов. Из чего следует, что количество элементов в измерении базовой матрицы равно количеству элементов во всей матрице.

Доступ к элементам измерения через []. 
Доступ к измерениям через () с ленивой инициализацией измерений. 
