#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Инициализируем приложение Qt и передаём ему аргументы командной строки.
    QApplication a(argc, argv);

    // Создаём и показываем главное окно программы.
    MainWindow w;
    w.show();

    // Запускаем цикл обработки событий Qt; он работает, пока приложение не завершится.
    return a.exec();
}
