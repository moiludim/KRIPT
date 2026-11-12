#include "substitutioncipher.h"

bool SubstitutionCipher::transform(const QString &text,
                                   const QString &alphabet,
                                   const QString &key,
                                   bool decrypt,
                                   QString &result,
                                   QString &error)
{
    // Пустой алфавит не позволяет сопоставить символы текста с символами ключа.
    if (alphabet.isEmpty()) {
        error = QStringLiteral("Алфавит не должен быть пустым.");
        return false;
    }

    // Проверяем, что алфавит состоит из уникальных строчных символов без пробелов.
    for (qsizetype i = 0; i < alphabet.size(); ++i) {
        const QChar symbol = alphabet.at(i);
        if (symbol.isSpace() || symbol != symbol.toLower() || alphabet.indexOf(symbol) != i) {
            error = QStringLiteral("Алфавит должен содержать только уникальные строчные символы без пробелов.");
            return false;
        }
    }

    // Ключ должен иметь длину алфавита, чтобы задавать полную подстановку.
    if (key.size() != alphabet.size()) {
        error = QStringLiteral("Ключ должен содержать столько же символов, сколько алфавит.");
        return false;
    }

    // Убеждаемся, что ключ — перестановка символов алфавита без повторов.
    for (qsizetype i = 0; i < key.size(); ++i) {
        const QChar symbol = key.at(i);
        if (symbol.isSpace() || symbol != symbol.toLower()
            || alphabet.indexOf(symbol) == -1 || key.indexOf(symbol) != i) {
            error = QStringLiteral("Ключ должен быть перестановкой символов алфавита в нижнем регистре.");
            return false;
        }
    }

    // Подготавливаем буфер результата; пробелы сохраняются, остальные символы заменяются.
    result.clear();
    result.reserve(text.size());

    // Для каждого символа находим соответствие в алфавите/ключе в зависимости от режима.
    for (const QChar symbol : text) {
        if (symbol.isSpace()) {
            result.append(symbol);
            continue;
        }

        // Поиск выполняется в нижнем регистре, а регистр исходного символа сохраняется ниже.
        const QChar normalized = symbol.toLower();
        const qsizetype index = decrypt ? key.indexOf(normalized) : alphabet.indexOf(normalized);
        if (index == -1) {
            error = QStringLiteral("Символ «%1» отсутствует в алфавите.").arg(symbol);
            result.clear();
            return false;
        }

        // При шифровании берём символ ключа, при расшифровании — соответствующий символ алфавита.
        QChar replacement = decrypt ? alphabet.at(index) : key.at(index);
        if (symbol.isUpper())
            replacement = replacement.toUpper();
        result.append(replacement);
    }

    // Все символы преобразованы успешно: очищаем сообщение об ошибке.
    error.clear();
    return true;
}
