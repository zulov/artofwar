#pragma once

#include <map>
#include <string>
#include <mutex>

class Count {
public:
    static void x();
    static void y();
    static void inc(const std::string& name);

    static void print_x2y();
    static void print_counters();

private:
    Count() = delete;

    static int counterX;
    static int counterY;
    static std::map<std::string, int> counters;
};
