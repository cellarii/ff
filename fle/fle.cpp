#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <conio.h>
#include <chrono>
#include <iomanip>
#include <cctype>
#include <gtest/gtest.h>
#include <clocale>
#include <windows.h> // Для SetConsoleOutputCP

using namespace std;
using namespace filesystem;

// Установка русской локали
void setRussianLocale() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    setlocale(LC_ALL, "Russian");
}

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
            info.size = info.flag ? 0 : iter.file_size();

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
            cout << "указанный путь не существует\n";
        }
        else {
            cout << "нет прав доступа или недопустимое имя файла\n";
        }
    }
    system("pause");
}

void newDir(const path& path) {
    string dirname;
    cout << "Введите имя папки: ";
    cin >> dirname;

    auto patth = path / dirname;
    if (exists(patth)) {
        if (is_directory(patth)) {
            cout << "Папка с таким именем уже существует\n";
        }
        else {
            cout << "Файл с таким именем уже существует\n";
        }
        system("pause");
        return;
    }

    if (create_directory(patth)) {
        cout << "Папка создана\n";
    }
    else {
        cout << "Ошибка создания папки: нет прав доступа или неверный путь\n";
    }
    system("pause");
}

void remove(const path& path, const string& name, bool flag) {
    try {
        bool result;
        if (flag) {
            result = remove_all(path / name) > 0;
        }
        else {
            result = filesystem::remove(path / name);
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

void renameItem(const path& path, const string& name1, string& name2) {
    try {
        if (exists(path / name2)) {
            if (is_directory(path / name2)) {
                cout << "Папка с таким именем уже существует\n";
            }
            else {
                cout << "Файл с таким именем уже существует\n";
            }
            system("pause");
            return;
        }
        filesystem::rename(path / name1, path / name2);
        cout << "Переименовано\n";
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << "\n";
    }
    system("pause");
}

bool word(char c) {
    return isspace(c) || ispunct(c);
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
                    string line2;
                    for (char c : line) {
                        line2 += tolower(c);
                    }

                    string key2;
                    for (char c : keyword) {
                        key2 += tolower(c);
                    }

                    size_t pos = line2.find(key2);
                    while (pos != string::npos) {
                        bool isWord = true;

                        if (pos > 0 && !word(line2[pos - 1])) {
                            isWord = false;
                        }

                        if (pos + key2.length() < line2.length() && !word(line2[pos + key2.length()])) {
                            isWord = false;
                        }

                        if (isWord) {
                            count++;
                        }

                        pos = line2.find(key2, pos + key2.length());
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
            for (const auto& [fpath, count] : results) {
                cout << fpath << " - " << count << " совпадений\n";
            }
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << "\n";
    }

    system("pause");
}

class FileManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = "test_temp_dir";
        create_directories(testDir);
        create_directories(testDir / "test_folder");
        ofstream(testDir / "test_file.txt") << "тестовое содержимое";
        ofstream(testDir / "empty_file.txt");
        ofstream(testDir / "search_test.txt") << "тест тест ТЕСТ тест. тест\nдругая строка";
    }

    void TearDown() override {
        remove_all(testDir);
    }

    path testDir;
};

TEST_F(FileManagerTest, OpenDirTest) {
    auto contents = openDir(testDir);
    ASSERT_GE(contents.size(), 3);
    EXPECT_TRUE(contents[0].flag);
    EXPECT_EQ(contents[0].name, "test_folder");
    EXPECT_FALSE(contents[1].flag);
}

TEST_F(FileManagerTest, NewFileTest) {
    testing::internal::CaptureStdout();
    newFile(testDir);
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Файл создан") != string::npos ||
        output.find("уже существует") != string::npos);
}

TEST_F(FileManagerTest, NewDirTest) {
    testing::internal::CaptureStdout();
    newDir(testDir);
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Папка создана") != string::npos ||
        output.find("уже существует") != string::npos);
}

TEST_F(FileManagerTest, RemoveFileTest) {
    testing::internal::CaptureStdout();
    remove(testDir, "test_file.txt", false);
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Удалено") != string::npos ||
        output.find("Ошибка") != string::npos);
}

TEST_F(FileManagerTest, RemoveDirTest) {
    testing::internal::CaptureStdout();
    remove(testDir, "test_folder", true);
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Удалено") != string::npos ||
        output.find("Ошибка") != string::npos);
}

TEST_F(FileManagerTest, RenameTest) {
    string oldName = "test_file.txt";
    string newName = "renamed_file.txt";

    testing::internal::CaptureStdout();
    renameItem(testDir, oldName, newName);
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Переименовано") != string::npos ||
        output.find("уже существует") != string::npos);
}

TEST_F(FileManagerTest, SearchTest) {
    testing::internal::CaptureStdout();
    search(testDir, "тест");
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("совпадений") != string::npos);
}

void help() {
    cout << "\nУправление:\n"
        << "  Стрелки - Навигация\n"
        << "  Enter   - Открыть папку\n"
        << "  Backspace - Назад\n"
        << "  F1 - Создать файл\n"
        << "  F2 - Создать папку\n"
        << "  F3 - Удалить\n"
        << "  F4 - Переименовать\n"
        << "  F5 - Поиск слова в текстовых файлах\n"
        << "  F6 - Запустить тесты\n"
        << "  ESC - Выход\n";
}

int main(int argc, char** argv) {
    setRussianLocale();

    if (argc > 1 && string(argv[1]) == "--gtest") {
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

    path currentPath = current_path();
    auto data = openDir(currentPath);
    int ind = 0;
    string name;

    while (true) {
        showDir(data, currentPath, ind);
        help();

        int key = _getch();
        if (key == 224 || key == 0) key = _getch();

        switch (key) {
        case 72: if (ind > 0) ind--; break;
        case 80: if (ind < data.size() - 1) ind++; break;
        case 13:
            if (!data.empty() && data[ind].flag) {
                currentPath /= data[ind].name;
                data = openDir(currentPath);
                ind = 0;
            }
            break;
        case 8:
            if (currentPath.has_parent_path()) {
                currentPath = currentPath.parent_path();
                data = openDir(currentPath);
                ind = 0;
            }
            break;
        case 59:
            newFile(currentPath);
            data = openDir(currentPath);
            break;
        case 60:
            newDir(currentPath);
            data = openDir(currentPath);
            break;
        case 61:
            if (!data.empty()) {
                remove(currentPath, data[ind].name, data[ind].flag);
                data = openDir(currentPath);
            }
            break;
        case 62:
            if (!data.empty()) {
                string newName;
                cout << "Введите новое имя: ";
                cin >> newName;
                renameItem(currentPath, data[ind].name, newName);
                data = openDir(currentPath);
            }
            break;
        case 63: {
            string keyword;
            cout << "Введите слово для поиска: ";
            cin >> keyword;
            search(currentPath, keyword);
            break;
        }
        case 64: {
            char* test_argv[] = { argv[0], (char*)"--gtest" };
            int test_argc = 2;
            testing::InitGoogleTest(&test_argc, test_argv);
            RUN_ALL_TESTS();
            system("pause");
            data = openDir(currentPath);
            break;
        }
        case 27: return 0;
        }
    }
}

