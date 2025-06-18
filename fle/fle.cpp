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