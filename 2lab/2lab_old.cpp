#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
//split text
std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = s.find(delim);
    while (end != std::string::npos) {
        result.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delim, start);
    }
    result.push_back(s.substr(start));
    return result;
}

//код шенона фано трудоемкий так как нужно сортнуть по убыванию вероятности
//читаем алфавит и вероятности из файла
//структура для символа,вероятености и кода. sp - symbol probability
struct sp{
    std::string symbol;
    double probability;
    std::string code;
};

void shenon_fano(std::vector<sp> &data, int start, int end){
    if(start >= end) {
        return;
    }
    
    double total = 0.0;
    for (int i = start; i <= end; ++i) {
        total += data[i].probability;
    }

    double left_sum = 0.0;
    double best_difference = std::numeric_limits<double>::infinity();
    int mid = start;

    for (int i = start; i < end; ++i) {
        left_sum += data[i].probability;
        double difference = std::abs(left_sum - (total - left_sum));

        if (difference < best_difference) {
            best_difference = difference;
            mid = i;
        }
    }

    for(int i = start; i <= mid; i++){
        data[i].code += "1";
    }
    for(int i = mid + 1; i <= end; i++){
        data[i].code += "0";
    }
    shenon_fano(data, start, mid);
    shenon_fano(data, mid + 1, end); 

}

//характеристики
double l_sred(std::vector<sp> &data){
    double l_sred = 0.0;
    for (const auto& item : data) {
        l_sred += item.probability * item.code.length();
    }
    return l_sred;
}

double entropy(std::vector<sp> &data){
    double entropy = 0.0;
    for (const auto& item : data) {
        if (item.probability > 0) {
            entropy -= item.probability * std::log2(item.probability);
        }
    }
    return entropy;
}

double kraft_check(std::vector<sp> &data){
    double sum = 0.0;
    for (const auto& item : data) {
        sum += std::pow(2, -static_cast<double>(item.code.length()));
    }
    if (sum <= 1.0) {
        std::cout << "Неравенство Крафта выполнено." << std::endl;
    } else {
        std::cout << "Неравенство Крафта не выполнено." << std::endl;
    }
    return sum;
}


//менюшка для шеннона фано
void shenon_fano_menu(std::vector<sp> &data){
    while (true) {
        std::cout << "Выберите действие:" << std::endl;
        std::cout << "1. Показать коды символов" << std::endl;
        std::cout << "2. Вычислить среднюю длину кодового слова и избыточность и проверить неравенство Крафта" << std::endl;
        std::cout << "3. Выйти" << std::endl;
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::cout << "Коды символов:" << std::endl;
                for (const auto& item : data) {
                    std::cout << item.symbol << ": " << item.code << std::endl;
                }
                break;
            }
            case 2: {
                double avg_length = l_sred(data);
                double ent = entropy(data);
                std::cout << "Средняя длина кодового слова: " << avg_length << std::endl;
                std::cout << "Избыточность: " << avg_length - ent << std::endl;
                kraft_check(data);
                break;
            }
            case 3:
                return;
            default:
                std::cout << "Неправильный выбор" << std::endl;
        }
    }
}

//чтение файлов и кодирование текста
void read_files_for_fano(){
    //открываем файлы с алфавитом и вероятностями
    std::cout<<"Введите название файла с алфавитом: ";
    std::string alphabet_filename;
    std::cin >> alphabet_filename;
    std::cout<<"Введите название файла с вероятностями: ";
    std::string probabilities_filename;
    std::cin >> probabilities_filename;
    std::cout<<"Введите название файла с текстом для кодирования: ";
    std::string text_filename;
    std::cin >> text_filename;
    std::cout<<"Введите название файла для сохранения кода: ";
    std::string output_filename;
    std::cin >> output_filename;

    std::ifstream alphabet_file(alphabet_filename);
    std::ifstream probabilities_file(probabilities_filename);
    std::ifstream text_file(text_filename);

    if (!alphabet_file.is_open()) {
        std::cerr << "Не удалось открыть файл с алфавитом." << std::endl;
        return;
    }
    if (!probabilities_file.is_open()) {
        std::cerr << "Не удалось открыть файл с вероятностями." << std::endl;
        return;
    }
    if (!text_file.is_open()) {
        std::cerr << "Не удалось открыть файл с текстом." << std::endl;
        return;
    }

    std::string alphabet_line;
    std::string probabilities_line;
    std::getline(alphabet_file, alphabet_line);
    std::getline(probabilities_file, probabilities_line);

    auto symbols = split(alphabet_line, ',');
    auto probabilities = split(probabilities_line, ',');

    if (symbols.size() != probabilities.size()) {
        std::cerr << "Количество символов и вероятностей не совпадает.\n";
        return;
    }

    std::vector<sp> data;
    double probability_sum = 0.0;
    for (std::size_t i = 0; i < symbols.size(); ++i) {
        try {
            std::size_t parsed_length = 0;
            double probability = std::stod(probabilities[i], &parsed_length);
            if (parsed_length != probabilities[i].size() ||
                !std::isfinite(probability) || probability < 0.0) {
                std::cerr << "Вероятности должны быть неотрицательными числами.\n";
                return;
            }
            if (symbols[i].empty()) {
                std::cerr << "Символ не может быть пустым.\n";
                return;
            }
            for (const auto& item : data) {
                if (item.symbol == symbols[i]) {
                    std::cerr << "Символы алфавита должны быть уникальными.\n";
                    return;
                }
            }
            data.push_back({symbols[i], probability, ""});
            probability_sum += probability;
        } catch (const std::exception&) {
            std::cerr << "Не удалось прочитать вероятность: " << probabilities[i] << '\n';
            return;
        }
    }

    if (std::abs(1.0 - probability_sum) > 1e-2) {
        std::cerr << "Сумма вероятностей должна быть равна 1.\n";
        return;
    }

    std::sort(data.begin(), data.end(),
        [](const sp& a, const sp& b) {
            return a.probability > b.probability;
        });

    if (data.size() == 1) {
        data[0].code = "1";
    } else {
        shenon_fano(data, 0, static_cast<int>(data.size()) - 1);
    }

    std::string text_line;
    std::string encoded_text;

    while (std::getline(text_file, text_line)) {
        auto t_l = split(text_line, ' ');
        for (int i = 0; i < t_l.size(); i++) {

            bool found = false;

            for (auto item : data) {
                if (item.symbol == t_l[i]) {
                    encoded_text += item.code;
                    found = true;
                    break;
                }
            }

            if (!found) {
                std::cerr << "Символ не найден в алфавите: "
                        << t_l[i] << std::endl;
                return;
            }
        }
    }
    
    std::ofstream output_file;
    output_file.open(output_filename);
    if (!output_file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи результата." << std::endl;
        return;
    }
    std::cout<<encoded_text<<std::endl;
    output_file << encoded_text;
    if (!output_file) {
        std::cerr << "Не удалось записать закодированный текст в файл." << std::endl;
        return;
    }
    std::cout << "Закодированный текст сохранён в файл: " << output_filename << std::endl;
    output_file.close();
    
    shenon_fano_menu(data);

}

//структура дерева для хранения кодов и декодирования
struct tree{
    std::string symbol;
    tree* left;
    tree* right;
    tree(std::string s = ""): symbol(s), left(nullptr), right(nullptr) {}
};
//построение дерева кодирования
void build_tree(tree* root, const std::vector<sp>& data){
    for (const auto& item : data) {
        tree* node = root;
        for (auto bit : item.code) {
            if (bit == '0') {
                if (!node->left) node->left = new tree();
                node = node->left;
            } else {
                if (!node->right) node->right = new tree();
                node = node->right;
            }
        }
        node->symbol = item.symbol;
    }
}
//декодирование файла
void decode_file(){
    std::string alphabet_filename;
    std::string probabilities_filename;
    std::string encoded_filename;
    std::string output_filename;

    std::cout << "Введите название файла с алфавитом: ";
    std::cin >> alphabet_filename;

    std::cout << "Введите название файла с вероятностями: ";
    std::cin >> probabilities_filename;

    std::cout << "Введите название файла с закодированной последовательностью: ";
    std::cin >> encoded_filename;

    std::cout << "Введите название файла для сохранения результата: ";
    std::cin >> output_filename;

    // Открываем входные файлы
    std::ifstream alphabet_file(alphabet_filename);
    std::ifstream probabilities_file(probabilities_filename);
    std::ifstream encoded_file(encoded_filename);

    if (!alphabet_file.is_open()) {
        std::cerr << "Не удалось открыть файл с алфавитом.\n";
        return;
    }

    if (!probabilities_file.is_open()) {
        std::cerr << "Не удалось открыть файл с вероятностями.\n";
        return;
    }

    if (!encoded_file.is_open()) {
        std::cerr << "Не удалось открыть файл с закодированной последовательностью.\n";
        return;
    }

    // Читаем алфавит и вероятности
    std::string alphabet_line;
    std::string probabilities_line;

    std::getline(alphabet_file, alphabet_line);
    std::getline(probabilities_file, probabilities_line);

    auto symbols = split(alphabet_line, ',');
    auto probabilities = split(probabilities_line, ',');

    if (symbols.size() != probabilities.size()) {
        std::cerr << "Количество символов и вероятностей не совпадает.\n";
        return;
    }

    // Формируем таблицу символов и вероятностей
    std::vector<sp> data;
    double probability_sum = 0.0;

    for (std::size_t i = 0; i < symbols.size(); ++i) {
        try {
            std::size_t parsed_length = 0;
            double probability =
                std::stod(probabilities[i], &parsed_length);

            if (parsed_length != probabilities[i].size() ||
                !std::isfinite(probability) || probability < 0.0) {
                std::cerr << "Некорректная вероятность.\n";
                return;
            }

            if (symbols[i].empty()) {
                std::cerr << "Символ не может быть пустым.\n";
                return;
            }

            for (const auto& item : data) {
                if (item.symbol == symbols[i]) {
                    std::cerr << "Символы алфавита должны быть уникальными.\n";
                    return;
                }
            }

            data.push_back({symbols[i], probability, ""});
            probability_sum += probability;
        }
        catch (const std::exception&) {
            std::cerr << "Не удалось прочитать вероятность: "
                      << probabilities[i] << '\n';
            return;
        }
    }

    if (std::abs(1.0 - probability_sum) > 1e-2) {
        std::cerr << "Сумма вероятностей должна быть равна 1.\n";
        return;
    }

    // Сортируем символы по убыванию вероятности
    std::sort(data.begin(), data.end(),
        [](const sp& a, const sp& b) {
            return a.probability > b.probability;
        });

    // Строим коды Шеннона—Фано
    if (data.size() == 1) {
        data[0].code = "1";
    }
    else {
        shenon_fano(data, 0, static_cast<int>(data.size()) - 1);
    }

    // Строим дерево кодирования
    tree* root = new tree();

    build_tree(root, data);

    // Читаем закодированную последовательность
    std::string coded_text;
    std::string decoded_text;
    while(std::getline(encoded_file, coded_text)){
        // Декодируем последовательность по дереву
        tree* current = root;

        for (char bit : coded_text) {
            if (bit != '0' && bit != '1') {
                if (std::isspace(static_cast<unsigned char>(bit))) {
                    continue;
                }

                std::cerr << "В закодированном файле найден недопустимый символ: "
                        << bit << '\n';
                return;
            }

            if (bit == '0') {
                current = current->left;
            }
            else {
                current = current->right;
            }

            if (current == nullptr) {
                std::cerr << "Некорректная последовательность битов.\n";
                return;
            }

            // Если дошли до листа, получили символ
            if (current->left == nullptr && current->right == nullptr) {
                decoded_text += current->symbol + ' ';
                current = root;
            }
            
        }
        // Проверяем, закончилась ли последовательность на границе кода
        if (current != root) {
            std::cerr << "Закодированная последовательность неполная.\n";
            return;
        }
    }
    
    // Сохраняем результат
    std::ofstream output_file;
    output_file.open(output_filename);

    if (!output_file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи результата.\n";
        return;
    }

    output_file << decoded_text;
    output_file.close();

    if (!output_file) {
        std::cerr << "Не удалось записать декодированный текст в файл.\n";
        return;
    }

    std::cout << "Декодированный текст:\n"
              << decoded_text << '\n';

    std::cout << "Декодированный текст сохранён в файл: "
              << output_filename << '\n';
}


void show_distribution_results(const std::string& name, std::vector<sp>& data){

    std::cout << "\n====================================\n";
    std::cout << name << '\n';
    std::cout << "====================================\n";

    std::cout << "Символ\tВероятность\tКод\tДлина\n";

    for (const auto& item : data) {
        std::cout << item.symbol << "\t"
                  << item.probability << "\t\t"
                  << item.code << "\t"
                  << item.code.length() << '\n';
    }

    double avg_length = l_sred(data);
    double ent = entropy(data);

    std::cout << "\nСредняя длина кодового слова: "
              << avg_length << " бит\n";

    std::cout << "Энтропия источника: "
              << ent << " бит\n";

    std::cout << "Избыточность: "
              << avg_length - ent << " бит\n";

    // Проверка неравенства Крафта
    double kraft_sum = kraft_check(data);

    std::cout << "Сумма по неравенству Крафта: "
              << kraft_sum << '\n';
}

void compare_distributions() {
    std::string alphabet_filename;
    std::string p1_filename;
    std::string p2_filename;
    std::size_t sequence_length;

    std::cout << "Введите название файла с алфавитом: ";
    std::cin >> alphabet_filename;

    std::cout << "Введите файл распределения P1(A): ";
    std::cin >> p1_filename;

    std::cout << "Введите файл распределения P2(A): ";
    std::cin >> p2_filename;

    std::cout << "Введите длину моделируемой последовательности: ";
    if (!(std::cin >> sequence_length) || sequence_length == 0) {
        std::cerr << "Длина последовательности должна быть положительным числом.\n";
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    // Открываем файлы
    std::ifstream alphabet_file(alphabet_filename);
    std::ifstream p1_file(p1_filename);
    std::ifstream p2_file(p2_filename);

    if (!alphabet_file.is_open() ||
        !p1_file.is_open() ||
        !p2_file.is_open()) {
        std::cerr << "Не удалось открыть один из входных файлов.\n";
        return;
    }

    // Читаем алфавит и вероятности
    std::string alphabet_line;
    std::string p1_line;
    std::string p2_line;

    std::getline(alphabet_file, alphabet_line);
    std::getline(p1_file, p1_line);
    std::getline(p2_file, p2_line);

    auto symbols = split(alphabet_line, ',');
    auto p1_probabilities = split(p1_line, ',');
    auto p2_probabilities = split(p2_line, ',');

    if (symbols.empty() ||
        symbols.size() != p1_probabilities.size() ||
        symbols.size() != p2_probabilities.size()) {
        std::cerr << "Количество символов и вероятностей не совпадает.\n";
        return;
    }

    // Создаём три вектора структур sp
    std::vector<sp> uniform;
    std::vector<sp> p1;
    std::vector<sp> p2;

    double uniform_probability = 1.0 / symbols.size();
    double sum1 = 0.0;
    double sum2 = 0.0;

    for (std::size_t i = 0; i < symbols.size(); ++i) {
        try {
            double probability1 = std::stod(p1_probabilities[i]);
            double probability2 = std::stod(p2_probabilities[i]);

            if (symbols[i].empty() ||
                !std::isfinite(probability1) ||
                !std::isfinite(probability2) ||
                probability1 < 0.0 || probability2 < 0.0) {
                std::cerr << "Некорректные данные.\n";
                return;
            }

            for (std::size_t j = 0; j < i; ++j) {
                if (symbols[i] == symbols[j]) {
                    std::cerr << "Символы должны быть уникальными.\n";
                    return;
                }
            }

            uniform.push_back({symbols[i], uniform_probability, ""});
            p1.push_back({symbols[i], probability1, ""});
            p2.push_back({symbols[i], probability2, ""});

            sum1 += probability1;
            sum2 += probability2;
        }
        catch (const std::exception&) {
            std::cerr << "Ошибка чтения вероятностей.\n";
            return;
        }
    }

    if (std::abs(sum1 - 1.0) > 1e-2 ||
        std::abs(sum2 - 1.0) > 1e-2) {
        std::cerr << "Сумма вероятностей должна быть равна 1.\n";
        return;
    }

    // Сортируем и строим коды Шеннона—Фано
    std::vector<sp>* distributions[] = {&uniform, &p1, &p2};

    for (auto distribution : distributions) {
        std::sort(distribution->begin(), distribution->end(),
            [](const sp& a, const sp& b) {
                return a.probability > b.probability;
            });

        if (distribution->size() == 1) {
            (*distribution)[0].code = "1";
        }
        else {
            shenon_fano(
                *distribution, 0,
                static_cast<int>(distribution->size()) - 1);
        }
    }
    
    // Вывод результатов для трёх распределений
    std::cout << "\n\nРЕЗУЛЬТАТЫ ДЛЯ ТРЁХ РАСПРЕДЕЛЕНИЙ\n";

    show_distribution_results("Равномерное распределение", uniform);
    show_distribution_results("Распределение P1(A)", p1);
    show_distribution_results("Распределение P2(A)", p2);

    // Моделируем последовательности и сравниваем длины кодов
    std::random_device rd;
    std::mt19937 generator(rd());

    std::vector<std::string> names = {
        "Равномерное", "P1(A)", "P2(A)"
    };

    std::cout << "\nДлины закодированных последовательностей (в битах), N = "
              << sequence_length << ":\n";

    std::cout << "Источник\tРавномерное\tP1(A)\t\tP2(A)\n";

    for (int source = 0; source < 3; ++source) {
        std::vector<sp>* source_distribution = distributions[source];

        std::vector<double> probabilities;
        for (const auto& item : *source_distribution) {
            probabilities.push_back(item.probability);
        }

        std::discrete_distribution<int> distribution(
            probabilities.begin(), probabilities.end());

        std::vector<std::string> sequence;

        for (std::size_t i = 0; i < sequence_length; ++i) {
            int index = distribution(generator);
            sequence.push_back((*source_distribution)[index].symbol);
        }

        std::cout << names[source];

        for (int coding = 0; coding < 3; ++coding) {
            std::size_t encoded_length = 0;

            for (const auto& symbol : sequence) {
                for (const auto& item : *distributions[coding]) {
                    if (item.symbol == symbol) {
                        encoded_length += item.code.size();
                        break;
                    }
                }
            }

            std::cout << "\t\t" << encoded_length;
        }

        std::cout << '\n';
    }
}

void menu(){
    while(true){
        std::cout<<"1. Кодировать"<<std::endl;
        std::cout<<"2. Декодировать"<<std::endl;
        std::cout<<"3. Работа с распределениями"<<std::endl;
        std::cout<<"4. Выход"<<std::endl;


        int choice;
        std::cin>>choice;
        switch(choice){
            case 1:
                std::cin.ignore();
                read_files_for_fano();
                break;
            case 2:
                std::cin.ignore();
                decode_file();
                return;
            case 3:
                std::cin.ignore();
                std::cout<<"Работа с распределениями"<<std::endl;
                compare_distributions();
                return;
            case 4:
                std::cout<<"Выход из программы"<<std::endl;
                return;
            default:
                std::cout<<"Неправильный выбор"<<std::endl;
                break;
        }
    }
}


int main(){
    
    menu();

}
