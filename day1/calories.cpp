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

    std::cout << std::format("Elf #{} is carrying {} food items with a total of {} calories.\n", index, elfs[index].foods.size(), elfs[index].calories());

    std::cout << "\nPart 2:\n";

    std::ranges::partial_sort(elfs, elfs.begin() + 3, std::greater{}, &Elf::calories);

    uint64_t sum = 0;
    for(int i = 0; i < 3; ++i) {
        sum += elfs[i].calories();
    }
    std::cout << std::format("The three elfs with the most calories are carrying {}, {} and {} calories. That are in total {} calories.\n",
                             elfs[0].calories(), elfs[1].calories(), elfs[2].calories(), sum);

    return 0;
}
