#include "mainwindow.h"
#include "substitutioncipher.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Создаём интерфейс и связываем кнопки с обработчиками действий.
    ui->setupUi(this);

    connect(ui->inputBrowseButton, &QPushButton::clicked, this, &MainWindow::browseInputFile);
    connect(ui->keyBrowseButton, &QPushButton::clicked, this, &MainWindow::browseKeyFile);
    connect(ui->alphabetBrowseButton, &QPushButton::clicked, this, &MainWindow::browseAlphabetFile);
    connect(ui->outputBrowseButton, &QPushButton::clicked, this, &MainWindow::browseOutputFile);
    connect(ui->encryptButton, &QPushButton::clicked, this, &MainWindow::encryptFile);
    connect(ui->decryptButton, &QPushButton::clicked, this, &MainWindow::decryptFile);
}

MainWindow::~MainWindow()
{
    // Освобождаем объект интерфейса, созданный в конструкторе.
    delete ui;
}

void MainWindow::browseInputFile()
{
    // Открываем диалог выбора исходного текстового файла и показываем путь в поле ввода.
    const QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("Выберите файл с текстом"), QString(),
        QStringLiteral("Текстовые файлы (*.txt);;Все файлы (*)"));
    if (!path.isEmpty())
        ui->inputPathEdit->setText(path);
}

void MainWindow::browseKeyFile()
{
    // Открываем диалог выбора файла ключа и показываем выбранный путь.
    const QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("Выберите файл с ключом"), QString(),
        QStringLiteral("Текстовые файлы (*.txt);;Все файлы (*)"));
    if (!path.isEmpty())
        ui->keyPathEdit->setText(path);
}

void MainWindow::browseAlphabetFile()
{
    // Открываем диалог выбора файла алфавита и показываем выбранный путь.
    const QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("Выберите файл с алфавитом"), QString(),
        QStringLiteral("Текстовые файлы (*.txt);;Все файлы (*)"));
    if (!path.isEmpty())
        ui->alphabetPathEdit->setText(path);
}

void MainWindow::browseOutputFile()
{
    // Открываем диалог задания файла, в который будет сохранён результат.
    const QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("Сохранить результат"), QString(),
        QStringLiteral("Текстовые файлы (*.txt);;Все файлы (*)"));
    if (!path.isEmpty())
        ui->outputPathEdit->setText(path);
}

void MainWindow::encryptFile()
{
    // Запускаем обработку в режиме шифрования.
    processFile(false);
}

void MainWindow::decryptFile()
{
    // Запускаем обработку в режиме расшифрования.
    processFile(true);
}

void MainWindow::processFile(bool decrypt)
{
    // Получаем пути из полей интерфейса и убираем пробелы по краям.
    const QString inputPath = ui->inputPathEdit->text().trimmed();
    const QString keyPath = ui->keyPathEdit->text().trimmed();
    const QString alphabetPath = ui->alphabetPathEdit->text().trimmed();
    const QString outputPath = ui->outputPathEdit->text().trimmed();

    // Без всех четырёх путей дальнейшая обработка невозможна.
    if (inputPath.isEmpty() || keyPath.isEmpty() || alphabetPath.isEmpty() || outputPath.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Не заполнены пути"),
                             QStringLiteral("Укажите файлы текста, ключа и алфавита, а также файл результата."));
        return;
    }

    // Читаем исходный текст в UTF-8; при ошибке открытия сообщаем о ней и завершаем операцию.
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, QStringLiteral("Ошибка чтения"),
                              QStringLiteral("Не удалось открыть входной файл:\n%1").arg(inputFile.errorString()));
        return;
    }
    const QString text = QString::fromUtf8(inputFile.readAll());
    inputFile.close();

    // Загружаем ключ из файла, удаляя лишние пробелы и переводы строк по краям.
    QFile keyFile(keyPath);
    if (!keyFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, QStringLiteral("Ошибка чтения"),
                              QStringLiteral("Не удалось открыть файл ключа:\n%1").arg(keyFile.errorString()));
        return;
    }
    const QString key = QString::fromUtf8(keyFile.readAll()).trimmed();
    keyFile.close();

    // Загружаем алфавит из файла в том же формате.
    QFile alphabetFile(alphabetPath);
    if (!alphabetFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, QStringLiteral("Ошибка чтения"),
                              QStringLiteral("Не удалось открыть файл алфавита:\n%1").arg(alphabetFile.errorString()));
        return;
    }
    const QString alphabet = QString::fromUtf8(alphabetFile.readAll()).trimmed();
    alphabetFile.close();

    // Передаём данные алгоритму шифрования/расшифрования; при ошибке результат не записываем.
    QString result;
    QString error;
    if (!SubstitutionCipher::transform(text, alphabet, key, decrypt, result, error)) {
        QMessageBox::warning(this, QStringLiteral("Не удалось обработать текст"), error);
        return;
    }

    // Абсолютный путь используем напрямую, относительный — считаем от каталога проекта.
    const QString resolvedOutputPath = QDir::isAbsolutePath(outputPath)
        ? outputPath
        : QDir(QStringLiteral(LAB4_PROJECT_DIR)).filePath(outputPath);
    // Открываем файл результата с очисткой прежнего содержимого.
    QFile outputFile(resolvedOutputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(this, QStringLiteral("Ошибка записи"),
                              QStringLiteral("Не удалось открыть файл результата:\n%1").arg(outputFile.errorString()));
        return;
    }

    // Кодируем результат в UTF-8 и проверяем, что записаны все байты.
    const QByteArray output = result.toUtf8();
    if (outputFile.write(output) != output.size()) {
        QMessageBox::critical(this, QStringLiteral("Ошибка записи"),
                              QStringLiteral("Не удалось полностью записать результат:\n%1").arg(outputFile.errorString()));
        return;
    }

    ui->statusbar->showMessage(
        decrypt ? QStringLiteral("Расшифрованный текст сохранён.")
                : QStringLiteral("Зашифрованный текст сохранён."),
        5000);
}
