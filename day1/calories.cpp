#include <algorithm>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct Elf {
    std::vector<uint64_t> foods;

    Elf() noexcept { foods.reserve(8); }
    [[nodiscard]] uint64_t calories() const noexcept { return std::accumulate(foods.begin(), foods.end(), 0ULL); }
};

int main()
{
    std::vector<Elf> elfs;
    elfs.reserve(1024);
    elfs.emplace_back();

    std::ifstream file("calories_input.txt");
    std::string line;
    while(std::getline(file, line)) {
        if (line.empty()) {
            elfs.emplace_back();
        }
        else {
            elfs.back().foods.emplace_back(std::stoi(line));
        }
    }

    auto it = std::ranges::max_element(elfs, {}, &Elf::calories);
    auto index = std::distance(elfs.begin(), it);

    std::cout << std::format("Elf #{} is carring {} food items with a total of {} calories.\n", index, elfs[index].foods.size(), elfs[index].calories());

    return 0;
}
