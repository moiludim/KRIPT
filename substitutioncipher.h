#pragma once

#include <QString>

class SubstitutionCipher
{
public:
    // Преобразует текст по алфавиту и ключу; decrypt выбирает режим, а result/error возвращают итог и ошибку.
    static bool transform(const QString &text,
                          const QString &alphabet,
                          const QString &key,
                          bool decrypt,
                          QString &result,
                          QString &error);
};
