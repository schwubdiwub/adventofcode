#include <algorithm>
#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

struct Range {
    uint64_t lowerBound;
    uint64_t upperBound;

    bool valid() const noexcept { return lowerBound <= upperBound; }
    bool contains(const Range& other) const noexcept
    {
        return lowerBound <= other.lowerBound & upperBound >= other.upperBound;
    }

    bool containsAtLeastOneBound(const Range& other) const noexcept
    {
        const bool containsLowerBound = lowerBound <= other.lowerBound & upperBound >= other.lowerBound;
        const bool containsUpperBound = lowerBound <= other.upperBound & upperBound >= other.upperBound;
        return containsLowerBound | containsUpperBound;
    }
};

using RangePair = std::pair<Range, Range>;
bool oneContainsToOther(const RangePair& rangePair) noexcept
{
    return rangePair.first.contains(rangePair.second) | rangePair.second.contains(rangePair.first);
}

bool overlapEachOther(const RangePair& rangePair) noexcept
{
    return rangePair.first.containsAtLeastOneBound(rangePair.second) | rangePair.second.containsAtLeastOneBound(rangePair.first);
}

std::vector<RangePair> parseCleaningInput(std::string_view filepath) noexcept
{
    std::vector<RangePair> result;
    result.reserve(1024);

    std::ifstream file(filepath.data());
    std::string line;

    std::string_view numberExpression = "([1-9][0-9]*)";
    std::regex lineRegex(std::format("{0}-{0},{0}-{0}", numberExpression));
    std::smatch subMatches;

    while(std::getline(file, line)) {
        if(std::regex_match(line, subMatches, lineRegex)) [[likely]] {
            assert(subMatches.size() == 5);

            result.emplace_back(Range{ std::stoull(subMatches[1].str()),  std::stoull(subMatches[2].str()) },
                                 Range{ std::stoull(subMatches[3].str()), std::stoull(subMatches[4].str()) });

            assert(result.back().first.valid());
            assert(result.back().second.valid());
        }
        else {
            assert(false && "Unexpected input format.");
        }
    }

    std::cout << std::format("Parsed {} pairs of ranges\n", result.size());

    return result;
}

int main()
{
    const auto rangePairs = parseCleaningInput("campcleaning_input.txt");

    const auto containsOtherCount = std::ranges::count_if(rangePairs, &oneContainsToOther);

    std::cout << std::format("There are {} assigned range pairs where one contains the other range.\n", containsOtherCount);

    std::cout << "Part 2:\n";

    const auto overlapCount = std::ranges::count_if(rangePairs, &overlapEachOther);

    std::cout << std::format("There are {} assigned range pairs that overlap with each other.\n", overlapCount);

    return 0;
}
