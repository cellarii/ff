#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <conio.h>
#include <chrono>
#include <iomanip>

using namespace std;

using namespace filesystem;

struct inf {
    string name;
    bool flag;
    long long size;
    string time;
};

bool sort2(const inf& a, const inf& b) {
    if (a.flag != b.flag) return a.flag > b.flag;
    return a.name < b.name;
}


vector<inf> openDir(const path& path) {
    vector<inf> data;
    try {
        for (const auto& iter : directory_iterator(path)) {
            inf info;
            info.name = iter.path().filename().string();
            info.flag = iter.is_directory();
            if (info.flag)
            {
                info.size = 0;
            }
            else
            {
                info.size = iter.file_size();
            }

            auto ftime = iter.last_write_time();
            auto stime = chrono::clock_cast<chrono::system_clock>(ftime);
            time_t timet = chrono::system_clock::to_time_t(stime);

            char timeStr[100];
            struct tm time;
            localtime_s(&time, &timet);
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &time);
            info.time = timeStr;

            data.push_back(info);
        }
        sort(data.begin(), data.end(), sort2);
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << "\n";
    }
    return data;
}

void showDir(const vector<inf>& data, const path& path, int ind) {
    system("cls");
    cout << "Текущая директория: " << path.string() << "\n\n";
    cout << left << setw(5) << ""
        << left << setw(35) << "Имя"
        << left << setw(10) << "Тип"
        << left << setw(15) << "Размер" << "\t"
        << "Дата изменения\n";

    for (size_t i = 0; i < data.size(); ++i) {
        cout << (i == ind ? "> " : "  ")
            << left << setw(35) << data[i].name
            << left << setw(10) << (data[i].flag ? "Папка" : "Файл")
            << left << setw(15) << (!data[i].flag ? to_string(data[i].size) : " ")
            << data[i].time << "\n";
    }
}

void newFile(const path& path) {
    string name;
    cout << "Введите имя файла: ";
    cin >> name;

    auto full = path / name;
    if (exists(full)) {
        cout << "Файл или папка с таким именем уже существует\n";
        system("pause");
        return;
    }
    ofstream file(full);

    if (file.is_open()) {
        cout << "Файл создан\n";
        file.close();
    }
    else {
        cout << "Ошибка создания файла: ";
        if (!exists(path)) {
            cout << " указанный путь не существует\n";
        }
        else {
            cout << " нет прав доступа или недопустимое имя файла\n";
        }
    }

    system("pause");
}

void newDir(const path& path) {
    string dirname;
    cout << "Введите имя папки: ";
    cin >> dirname;

    bool create = create_directory(path / dirname);

    if (create) {
        cout << "Папка создана\n";
    }
    else {
        cout << "Ошибка создания папки,  ";
        if (exists(path / dirname)) {
            cout << "  папка уже существует\n";
        }
        else {
            cout << " нет прав доступа или неверный путь\n";
        }
    }

    system("pause");
}

void remove(const path& path, const string& name, bool flag) {
    try {
        bool result;
        if (flag) {
            result = remove_all(path / name);
        }
        else {
            result = remove(path / name);
        }

        if (result) {
            cout << "Удалено\n";
        }
        else {
            cout << "Ошибка удаления\n";
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << "\n";
    }
    system("pause");
}

void rename(const path& path, const string& name1) {
    string name2;
    cout << "Введите новое имя: ";
    cin >> name2;
    try {
        rename(path / name1, path / name2);
        cout << "Переименовано\n";
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << "\n";
    }
    system("pause");
}

void search(const path& path, const string& keyword) {
    vector<pair<filesystem::path, int>> results;

    try {
        for (const auto& file : recursive_directory_iterator(path)) {
            if (file.is_regular_file() && file.path().extension() == ".txt") {
                ifstream inFile(file.path());
                string line;
                int count = 0;

                while (getline(inFile, line)) {
                    for (size_t pos = line.find(keyword); pos != string::npos; pos = line.find(keyword, pos + keyword.length())) {
                        count++;
                    }
                }

                if (count > 0) {
                    results.emplace_back(file.path(), count);
                }
            }
        }

        if (results.empty()) {
            cout << "Слово \"" << keyword << "\" не найдено.\n";
        }
        else {
            cout << "Результаты поиска для слова \"" << keyword << "\":\n";
            for (const auto& [filePath, count] : results) {
                cout << filePath << " - " << count << " совпадений\n";
            }
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << "\n";
    }

    system("pause");
}

void help() {
    cout << "\nУправление:\n"
        << "Стрелки - Навигация\n"
        << "Enter - Открыть папку\n"
        << "Backspace - Назад\n"
        << "F1 - Создать файл\n"
        << "F2 - Создать папку\n"
        << "F3 - Удалить\n"
        << "F4 - Переименовать\n"
        << "F5 - Поиск слова в текстовых файлах\n"
        << "ESC - Выход\n";
}

int main() {

    path path = "C:\\";
    auto data = openDir(path);
    int ind = 0;

    while (true) {
        showDir(data, path, ind);
        help();

        int key = _getch();
        if (key == 224 || key == 0) key = _getch();

        switch (key) {
        case 72:
            if (ind > 0)
            {
                ind--;
                break;
            }
        case 80:
            if (ind < data.size() - 1)
            {
                ind++;
                break;
            }
        case 13:
            if (!data.empty() && data[ind].flag) {
                path /= data[ind].name;
                data = openDir(path);
                ind = 0;
            }
            break;
    }
}