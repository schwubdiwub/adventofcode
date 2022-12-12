#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

int xLimit;
int yLimit;

struct Pos {
    int x, y;

    auto operator<=>(const Pos& other) const noexcept = default;

    [[nodiscard]] std::optional<Pos> up() const noexcept { Pos result{ x, y - 1 }; return result.y >= 0 ? std::optional{ result } : std::nullopt; }
    [[nodiscard]] std::optional<Pos> down() const noexcept { Pos result{ x, y + 1 }; return result.y < yLimit ? std::optional{ result } : std::nullopt; }
    [[nodiscard]] std::optional<Pos> left() const noexcept { Pos result{ x - 1, y }; return result.x >= 0 ? std::optional{ result } : std::nullopt; }
    [[nodiscard]] std::optional<Pos> right() const noexcept { Pos result{ x + 1, y }; return result.x < xLimit ? std::optional{ result } : std::nullopt; }
};

using Path = std::vector<Pos>;

struct HeightMap : public std::vector<std::vector<char>> {
    [[nodiscard]] char& height(const Pos& position) noexcept { return operator[](position.y)[position.x]; }
    [[nodiscard]] const char& height(const Pos& position) const noexcept { return operator[](position.y)[position.x]; }
};

struct VisitedMap : public std::vector<std::vector<bool>> {
    std::vector<std::vector<uint64_t>> distances;

    VisitedMap(size_t size, const std::vector<bool>& boolInitValue, const std::vector<uint64_t>& distanceInitValue) noexcept
        : std::vector<std::vector<bool>>(size, boolInitValue)
        , distances(size, distanceInitValue)
    {
    }

    void visitLocation(const Pos& position) noexcept { operator[](position.y)[position.x] = true; }
    [[nodiscard]] bool isVisited(const Pos& position) const noexcept { return operator[](position.y)[position.x]; }
    uint64_t& distance(const Pos& position) noexcept { return distances[position.y][position.x]; }
};

HeightMap parseHeightMap(std::string_view filepath) noexcept
{
    HeightMap heightMap;
    heightMap.reserve(64);

    std::ifstream file(filepath.data());
    std::string line;

    while (std::getline(file, line)) {
        if(line.empty()) {
            continue;
        }

        heightMap.emplace_back();
        heightMap.back().reserve(line.size());
        heightMap.back().insert(heightMap.back().end(), line.begin(), line.end());
    }

    return heightMap;
}

VisitedMap initVisitedMap(const HeightMap& heightMap) noexcept
{
    std::vector<bool> defaultVisited(heightMap[0].size(), false);
    std::vector<uint64_t> defaultDistance(heightMap[0].size(), std::numeric_limits<uint64_t>::max());

    VisitedMap visitedMap(heightMap.size(), defaultVisited, defaultDistance);

    return visitedMap;
}

Pos findFirstCharacterPosition(const HeightMap& heightMap, char character) noexcept
{
    for(size_t r = 0; r < heightMap.size(); ++r) {
        for(size_t c = 0; c < heightMap[r].size(); ++c) {
            auto foundIt = std::ranges::find(heightMap[r], character);
            if(foundIt != heightMap[r].end()) {
                return { static_cast<uint16_t>(std::distance(heightMap[r].begin(), foundIt)), static_cast<uint16_t>(r) };
            }
        }
    }

    assert(false);
    return { 0, 0 };
}

Pos findStartPosition(const HeightMap& heightMap) noexcept
{
    return findFirstCharacterPosition(heightMap, 'S');
}

Pos findDestinationPosition(const HeightMap& heightMap) noexcept
{
    return findFirstCharacterPosition(heightMap, 'E');
}

uint64_t findShortestPath(const HeightMap& heightMap, VisitedMap& visitedMap, const Pos& startPos, const Pos& destinationPos) noexcept
{
    std::vector<Pos> nextToCheck;
    nextToCheck.reserve(xLimit * yLimit);
    nextToCheck.emplace_back(startPos);
    visitedMap.distance(startPos) = 0;

    for(size_t checkIndex = 0; checkIndex < nextToCheck.size(); ++checkIndex) {
        const Pos& currentPos = nextToCheck[checkIndex];
        const auto height = heightMap.height(currentPos);
        const auto distance = visitedMap.distance(currentPos);

        if(currentPos == destinationPos) {
            return distance;
        }

        for(const auto& step : { currentPos.up(), currentPos.down(), currentPos.left(), currentPos.right() }) {
            if (step.has_value()) {
                if (not visitedMap.isVisited(*step) & ((height + 1) >= heightMap.height(*step))) {
                    nextToCheck.emplace_back(*step);
                    visitedMap.visitLocation(*step);
                    visitedMap.distance(*step) = distance + 1;
                }
            }
        }
    }

    return xLimit * yLimit;
}

int main()
{
//    HeightMap heightMap = parseHeightMap("findingsignal_input_test");
    HeightMap heightMap = parseHeightMap("findingsignal_input");

    xLimit = static_cast<int>(heightMap[0].size());
    yLimit = static_cast<int>(heightMap.size());
    const Pos start = findStartPosition(heightMap);
    const Pos destination = findDestinationPosition(heightMap);
    heightMap.height(start) = 'a';
    heightMap.height(destination) = 'z';

    {
        VisitedMap visitedMap = initVisitedMap(heightMap);
        const auto shortestPathLength = findShortestPath(heightMap, visitedMap, start, destination);

        std::cout << std::format("\nThe shortest path length is {}.\n", shortestPathLength);
    }

    {
        VisitedMap visitedMap = initVisitedMap(heightMap);
        uint64_t min_a_Length = std::numeric_limits<uint64_t>::max();

        for(Pos pos = { 0, 0 }; pos.x < xLimit; ++pos.x) {
            for(; pos.y < yLimit; ++pos.y) {
                if(heightMap.height(pos) == 'a') {
                    const uint64_t distance = findShortestPath(heightMap, visitedMap, pos, destination);
                    min_a_Length = std::min(min_a_Length, distance);
                    visitedMap = initVisitedMap(heightMap);
                }
            }
        }

        std::cout << std::format("\nThe shortest path length from the lowest height is {}.\n", min_a_Length);
    }



    return 0;
}
