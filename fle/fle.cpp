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