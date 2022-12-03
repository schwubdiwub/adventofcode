#include <algorithm>
#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

uint64_t itemPriority(char item) noexcept
{
    if(item >= 'a' & item <= 'z') {
        return item - 'a' + 1;
    }
    if(item >= 'A' & item <= 'Z') {
        return item - 'A' + 27;
    }
    return 0;
}

struct Rucksack {
    std::string _contents;
    std::string _duplicates;

    void findDuplicates() noexcept
    {
        assert(_contents.size() % 2 == 0);
        auto middle = _contents.begin() + std::ssize(_contents) / 2;
        for(auto it = _contents.begin(); it != middle; ++it) {
            for(auto jt = middle; jt != _contents.end(); ++jt) {
                if(*it == *jt) {
                    _duplicates.push_back(*it);
                    break;
                }
            }
        }
        auto removeRange = std::ranges::unique(_duplicates);
        _duplicates.erase(removeRange.begin(), removeRange.end());
    }

    [[nodiscard]] uint64_t duplicatePrioritiesSum() const noexcept
    {
        return std::accumulate(_duplicates.begin(), _duplicates.end(), 0ULL, [](uint64_t v, char c){ return v + itemPriority(c); });
    }
};

char findGroupItem(const Rucksack& first, const Rucksack& second, const Rucksack& third) noexcept
{
    for(auto it = first._contents.begin(); it != first._contents.end(); ++it) {
        const bool secondHasIt = std::ranges::find(second._contents, *it) != second._contents.end();
        const bool thirdHasIt = std::ranges::find(third._contents, *it) != third._contents.end();
        if(secondHasIt & thirdHasIt) {
            return *it;
        }
    }
    return '\0';
}

std::vector<Rucksack> parseRucksacks(std::string_view filepath) noexcept
{
    std::vector<Rucksack> rucksacks;
    rucksacks.reserve(4096);

    std::ifstream file(filepath.data());
    rucksacks.emplace_back();

    while(std::getline(file, rucksacks.back()._contents)) {
        rucksacks.emplace_back();
    }

    rucksacks.pop_back();

    return rucksacks;
}

int main()
{
    auto rucksacks = parseRucksacks("rucksacks_input.txt");

    for(auto& r : rucksacks) {
        r.findDuplicates();
    }

    const auto prioritiesSum = std::accumulate(rucksacks.begin(), rucksacks.end(), 0ULL, [](uint64_t v, const Rucksack& r){ return v + r.duplicatePrioritiesSum(); });
    std::cout << std::format("The sum of the priorities of the items in both compartments is: {}\n", prioritiesSum);

    std::cout << "Part 2:\n";

    uint64_t groupItemPrioritySum = 0;
    for(size_t i = 0; i < rucksacks.size(); i += 3) {
        const char groupItem = findGroupItem(rucksacks[i], rucksacks[i + 1], rucksacks[i + 2]);
        groupItemPrioritySum += itemPriority(groupItem);
    }

    std::cout << std::format("The sum of the priorities of the group items is: {}\n", groupItemPrioritySum);

    return 0;
}
