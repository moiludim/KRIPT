#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Создаёт главное окно и его интерфейс.
    explicit MainWindow(QWidget *parent = nullptr);
    // Освобождает ресурсы интерфейса.
    ~MainWindow();

private slots:
    // Обработчики выбора входного файла, ключа, алфавита и файла результата.
    void browseInputFile();
    void browseKeyFile();
    void browseAlphabetFile();
    void browseOutputFile();
    // Обработчики запуска шифрования и расшифрования.
    void encryptFile();
    void decryptFile();

private:
    // Выполняет чтение файлов, преобразование текста и запись результата.
    void processFile(bool decrypt);

    // Указатель на созданные Qt элементы интерфейса.
    Ui::MainWindow *ui;
};
