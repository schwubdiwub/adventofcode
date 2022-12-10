#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

struct Pos {
    int64_t x = 0, y = 0;

    auto operator<=>(const Pos& other) const noexcept = default;
};

struct Head {
    Pos pos;
};

struct Tail {
    Pos pos;
};

struct Rope {
    Head head;
    Tail tail;

    std::vector<Pos> tailPosHistory;

    [[nodiscard]] bool sameRow() const noexcept { return head.pos.y == tail.pos.y; }
    [[nodiscard]] bool sameCol() const noexcept { return head.pos.x == tail.pos.x; }
    [[nodiscard]] int64_t absRowDiff() const noexcept { return std::abs(head.pos.y - tail.pos.y); }
    [[nodiscard]] int64_t absColDiff() const noexcept { return std::abs(head.pos.x - tail.pos.x); }
    [[nodiscard]] int64_t getUniqueTailPosCount() noexcept;

    void moveHeadUp(int64_t count) noexcept;
    void moveHeadDown(int64_t count) noexcept;
    void moveHeadLeft(int64_t count) noexcept;
    void moveHeadRight(int64_t count) noexcept;

    void updateTailPos() noexcept;

    void moveTailTowardHeadInRow() noexcept;
    void moveTailTowardHeadInCol() noexcept;
};

int64_t Rope::getUniqueTailPosCount() noexcept
{
    std::ranges::sort(tailPosHistory);
    auto removeRange = std::ranges::unique(tailPosHistory);
    tailPosHistory.erase(removeRange.begin(), removeRange.end());
    return std::ssize(tailPosHistory);
}

void Rope::moveHeadUp(int64_t count) noexcept
{
    for(int64_t i = 0; i < count; ++i) {
        head.pos.y += 1;
        updateTailPos();
    }
}

void Rope::moveHeadDown(int64_t count) noexcept
{
    for(int64_t i = 0; i < count; ++i) {
        head.pos.y -= 1;
        updateTailPos();
    }
}

void Rope::moveHeadLeft(int64_t count) noexcept
{
    for(int64_t i = 0; i < count; ++i) {
        head.pos.x -= 1;
        updateTailPos();
    }
}

void Rope::moveHeadRight(int64_t count) noexcept
{
    for(int64_t i = 0; i < count; ++i) {
        head.pos.x += 1;
        updateTailPos();
    }
}

void Rope::updateTailPos() noexcept
{
    const auto absRowDifference = absRowDiff();
    const auto absColDifference = absColDiff();

    if(sameCol()) {
        if(absRowDifference > 1) {
            moveTailTowardHeadInCol();
            assert(absRowDiff() == 1);
        }
    }
    else if(sameRow()) {
        if(absColDifference > 1) {
            moveTailTowardHeadInRow();
            assert(absColDiff() == 1);
        }
    }
    else {
        const auto absDiffSum = absRowDifference + absColDifference;
        if(absDiffSum > 2) {
            moveTailTowardHeadInRow();
            moveTailTowardHeadInCol();

            const auto newAbsRowDiff = absRowDiff();
            const auto newAbsColDiff = absColDiff();
            assert(not (absDiffSum == 3) | ((newAbsRowDiff == 0 & newAbsColDiff == 1) | (newAbsColDiff == 0 & newAbsRowDiff == 1)));
            assert(not (absDiffSum == 4) | (newAbsRowDiff == 1 & newAbsColDiff == 1));
        }
    }

    tailPosHistory.emplace_back(tail.pos);
}

void Rope::moveTailTowardHeadInRow() noexcept
{
    if(head.pos.x > tail.pos.x) {
        tail.pos.x += 1;
    }
    else {
        tail.pos.x -= 1;
    }
}

void Rope::moveTailTowardHeadInCol() noexcept
{
    if(head.pos.y > tail.pos.y) {
        tail.pos.y += 1;
    }
    else {
        tail.pos.y -= 1;
    }
}

struct SeriesOfRopes {
    std::vector<Rope> ropes{ 9 };

    void moveHeadUp(int64_t count) noexcept;
    void moveHeadDown(int64_t count) noexcept;
    void moveHeadLeft(int64_t count) noexcept;
    void moveHeadRight(int64_t count) noexcept;

    void updateTailPositions() noexcept;

private:

    template <typename CommandType>
    void runCommand(CommandType command, int64_t count) noexcept
    {
        for(int64_t i = 0; i < count; ++i) {
            std::invoke(command, &ropes[0], 1);
            updateTailPositions();
        }
    }
};

void SeriesOfRopes::moveHeadUp(int64_t count) noexcept
{
    runCommand(&Rope::moveHeadUp, count);
}

void SeriesOfRopes::moveHeadDown(int64_t count) noexcept
{
    runCommand(&Rope::moveHeadDown, count);
}

void SeriesOfRopes::moveHeadLeft(int64_t count) noexcept
{
    runCommand(&Rope::moveHeadLeft, count);
}

void SeriesOfRopes::moveHeadRight(int64_t count) noexcept
{
    runCommand(&Rope::moveHeadRight, count);
}

void SeriesOfRopes::updateTailPositions() noexcept
{
    for(size_t i = 1; i < ropes.size(); ++i) {
        ropes[i].head.pos = ropes[i - 1].tail.pos;
        ropes[i].updateTailPos();
    }
}

template <typename RopeLike>
RopeLike parseAndRunInput(std::string_view filepath) noexcept
{
    RopeLike ropeLike;

    std::regex commandRegex("^([UDLR]) (\\d+)");
    std::smatch subMatches;

    std::ifstream file(filepath.data());
    std::string line;

    int lineCounter = 0;
    while (std::getline(file, line)) {
        const bool matched = std::regex_match(line, subMatches, commandRegex);
        assert(matched);

        const int64_t count = std::stoll(subMatches[2].str());
        const char commandChar = *subMatches[1].first;
        switch (commandChar) {
            case 'U': {
                ropeLike.moveHeadUp(count);
                break;
            }
            case 'D': {
                ropeLike.moveHeadDown(count);
                break;
            }
            case 'L': {
                ropeLike.moveHeadLeft(count);
                break;
            }
            case 'R': {
                ropeLike.moveHeadRight(count);
                break;
            }
            default: {
                std::cerr << "Invalid line syntax, cannot parse.\n";
                abort();
            }
        }
        ++lineCounter;
    }

    return ropeLike;
}

int main()
{
    Rope rope = parseAndRunInput<Rope>("ropephysics_input");

    std::cout << std::format("The tail was at {} different locations.\n", rope.getUniqueTailPosCount());

    std::cout << "Part 2:\n";

    auto seriesOfRopes = parseAndRunInput<SeriesOfRopes>("ropephysics_input");

    std::cout << std::format("The tail was at {} different locations.\n", seriesOfRopes.ropes.back().getUniqueTailPosCount());

    return 0;
}
