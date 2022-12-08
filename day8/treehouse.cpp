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

struct TreeGrid {

    std::vector<std::vector<uint16_t>> grid;
    size_t rows;
    size_t cols;

    void parseFromFile(std::string_view filepath) noexcept;

    bool isTreeFromOutsideVisible(size_t row, size_t col) const noexcept;
    size_t countFromOutsideVisibleTrees() const noexcept;
    size_t calcScenicScoreOfTree(int64_t row, int64_t col) const noexcept;
    size_t findMaximumScenicScore() const noexcept;
};

bool TreeGrid::isTreeFromOutsideVisible(size_t row, size_t col) const noexcept
{
    const auto treeHeight = grid[row][col];

    // check top
    {
        bool topVisible = true;
        for (size_t r = 0; r < row; ++r) {
            if (grid[r][col] >= treeHeight) {
                topVisible = false;
                break;
            }
        }
        if (topVisible) {
            return true;
        }
    }
    // check bottom
    {
        bool bottomVisible = true;
        for (size_t r = row + 1; r < rows; ++r) {
            if (grid[r][col] >= treeHeight) {
                bottomVisible = false;
                break;
            }
        }
        if (bottomVisible) {
            return true;
        }
    }
    // check left
    {
        bool leftVisible = true;
        for (size_t c = 0; c < col; ++c) {
            if (grid[row][c] >= treeHeight) {
                leftVisible = false;
                break;
            }
        }
        if (leftVisible) {
            return true;
        }
    }
    // check right
    {
        bool rightVisible = true;
        for (size_t c = col + 1; c < cols; ++c) {
            if (grid[row][c] >= treeHeight) {
                rightVisible = false;
                break;
            }
        }
        if (rightVisible) {
            return true;
        }
    }
    return false;
}

size_t TreeGrid::countFromOutsideVisibleTrees() const noexcept
{
    size_t count = 0;
    count += 2 * grid.size();
    count += 2 * (grid[0].size() - 2);

    for(size_t c = 1; c < cols - 1; ++c) {
        for(size_t r = 1; r < rows - 1; ++r) {
            if(isTreeFromOutsideVisible(r, c)) {
                ++count;
            }
        }
    }

    return count;
}

size_t TreeGrid::calcScenicScoreOfTree(int64_t row, int64_t col) const noexcept
{
    const auto treeHeight = grid[row][col];

    size_t topRange;
    size_t bottomRange;
    size_t leftRange;
    size_t rightRange;

    // calc top viewing distance
    {
        int64_t r = row - 1;
        for (; r >= 0; --r) {
            if (grid[r][col] >= treeHeight) {
                break;
            }
            if(r == 0) {
                break;
            }
        }
        topRange = row - r;
    }
    // calc bottom viewing distance
    {
        int64_t r = row + 1;
        for (; r < rows; ++r) {
            if (grid[r][col] >= treeHeight) {
                break;
            }
            if(r == rows - 1) {
                break;
            }
        }
        bottomRange = r - row;
    }
    // calc left viewing distance
    {
        int64_t c = col - 1;
        for (; c >= 0; --c) {
            if (grid[row][c] >= treeHeight) {
                break;
            }
            if(c == 0) {
                break;
            }
        }
        leftRange = col - c;
    }
    // calc right viewing distance
    {
        int64_t c = col + 1;
        for (; c < cols; ++c) {
            if (grid[row][c] >= treeHeight) {
                break;
            }
            if(c == cols - 1) {
                break;
            }
        }
        rightRange = c - col;
    }
    return topRange * bottomRange * leftRange * rightRange;
}

size_t TreeGrid::findMaximumScenicScore() const noexcept
{
    size_t maximumScore = std::numeric_limits<size_t>::min();

    for(size_t c = 1; c < cols - 1; ++c) {
        for(size_t r = 1; r < rows - 1; ++r) {
            const size_t score = calcScenicScoreOfTree(r, c);
            maximumScore = std::max(maximumScore, score);
        }
    }

    return maximumScore;
}

void TreeGrid::parseFromFile(std::string_view filepath) noexcept
{
    std::fstream file(filepath.data());
    std::string line;

    grid.reserve(128);

    while (std::getline(file, line)) {
        grid.emplace_back();
        grid.back().reserve(line.size());

        for(const auto& c : line) {
            grid.back().emplace_back(c - '0');
        }
    }

    rows = grid.size();
    cols = grid[0].size();
}

int main()
{
    TreeGrid treeGrid;
    treeGrid.parseFromFile("treehouse_input");

    std::cout << std::format("From the outside visible trees: {}\n", treeGrid.countFromOutsideVisibleTrees());
    std::cout << std::format("The maximum possible scenic score is: {}\n", treeGrid.findMaximumScenicScore());

    return 0;
}
