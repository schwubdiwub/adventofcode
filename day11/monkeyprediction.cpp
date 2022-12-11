#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

uint64_t ReliefDivisor = 0;

using namespace std::string_literals;

std::smatch assertRegexMatch(const std::string& stringToMatch, const std::string& regexString) noexcept
{
    std::regex regex(regexString);
    std::smatch subMatches;
    const bool matched = std::regex_match(stringToMatch, subMatches, regex);
    assert(matched);
    return subMatches;
}

struct Item {
    uint64_t worryingLevel;

    void applyRelief() noexcept { worryingLevel /= ReliefDivisor; }
};

struct Operation {
    enum class Op {
        mul,
        add
    };

    Op operation;
    std::optional<uint64_t> otherOperantValue;
    uint64_t moduloClass = std::numeric_limits<uint64_t>::max();

    [[nodiscard]] uint64_t calculateResult(uint64_t old) const noexcept;
};

uint64_t Operation::calculateResult(uint64_t old) const noexcept
{
    const uint64_t rhsOperantValue = otherOperantValue.has_value() ? otherOperantValue.value() : old;
    switch (operation) {
        case Op::mul: return (old * rhsOperantValue) % moduloClass;
        case Op::add: return (old + rhsOperantValue) % moduloClass;
    }
}

struct Monkey {
    Operation operation;
    uint64_t testDivisor;
    std::pair<size_t, size_t> throwDestinations;
    std::vector<Item> items;
    uint64_t itemInspectedCount = 0;

    void setFromLines(const std::array<std::string, 5>& lines) noexcept;

    std::pair<size_t, Item> handleNextItem() noexcept;

private:
    void applyOperation() noexcept;
    void applyRelief() noexcept;
    size_t executeTest(const Item& itemToTest) noexcept;
};

void Monkey::setFromLines(const std::array<std::string, 5>& lines) noexcept
{
    {
        std::smatch subMatches = assertRegexMatch(lines[0], R"(^  Starting items: (\d+)?.*)"s);

        if(subMatches.size() > 1) {
            items.emplace_back(std::stoull(subMatches[1].str()));

            std::string remainingItemsString(subMatches[1].second, lines[0].end());
            std::regex additionalItemRegex(R"(, (\d+))"s);
            while (not remainingItemsString.empty()) {
                std::smatch additionalSubMatches;
                const bool matched = std::regex_search(remainingItemsString, additionalSubMatches, additionalItemRegex);
                assert(matched);

                items.emplace_back(std::stoull(additionalSubMatches[1].str()));
                remainingItemsString = additionalSubMatches.suffix();
            }
        }
    }
    {
        std::smatch subMatches = assertRegexMatch(lines[1], R"(^  Operation: new = old (\+|\*) (\d+|old)$)"s);
        assert(subMatches.size() == 3);

        operation.operation = subMatches[1].str() == "*" ? Operation::Op::mul : Operation::Op::add;
        const std::string operantString = subMatches[2].str();
        operation.otherOperantValue = operantString == "old" ? std::nullopt : std::optional{ std::stoull(operantString) };
    }
    {
        std::smatch subMatches = assertRegexMatch(lines[2], R"(^  Test: divisible by (\d+)$)"s);
        assert(subMatches.size() == 2);

        testDivisor = std::stoull(subMatches[1].str());
    }
    {
        std::smatch subMatches = assertRegexMatch(lines[3], R"(^    If true: throw to monkey (\d+)$)"s);
        assert(subMatches.size() == 2);

        throwDestinations.first = std::stoull(subMatches[1].str());
    }
    {
        std::smatch subMatches = assertRegexMatch(lines[4], R"(^    If false: throw to monkey (\d+)$)"s);
        assert(subMatches.size() == 2);

        throwDestinations.second = std::stoull(subMatches[1].str());
    }
}

std::pair<size_t, Item> Monkey::handleNextItem() noexcept
{
    applyOperation();
    applyRelief();

    Item itemToThrow = items.front();
    items.erase(items.begin());

    return { executeTest(itemToThrow), itemToThrow };
}

void Monkey::applyOperation() noexcept
{
    Item& item = items.front();

//    const auto beforeOperationValue = item.worryingLevel;
    item.worryingLevel = operation.calculateResult(item.worryingLevel);

//    if(beforeOperationValue > item.worryingLevel) {
//        std::cout << std::format("Overflow detected: {} -> {}\n", beforeOperationValue, item.worryingLevel);
//    }
}

void Monkey::applyRelief() noexcept
{
    items.front().applyRelief();
    ++itemInspectedCount;
}

size_t Monkey::executeTest(const Item& itemToTest) noexcept
{
    return (itemToTest.worryingLevel % testDivisor == 0) ? throwDestinations.first : throwDestinations.second;
}

struct KeepAwaySimulation {
    std::vector<Monkey> monkeys;

    void parseStartState(std::string_view filepath) noexcept;

    void simulateRound() noexcept;
    void printInspectCounts() const noexcept;

    uint64_t calculateMonkeyBusiness() const noexcept;
};

void KeepAwaySimulation::parseStartState(std::string_view filepath) noexcept
{
    std::ifstream file(filepath.data());
    std::string line;

    std::regex monkeyRegex("^Monkey \\d:$");

    while (std::getline(file, line)) {
        const bool matched = std::regex_match(line, monkeyRegex);
        if(matched) {
            monkeys.emplace_back();

            std::array<std::string, 5> monkeyLines;
            for(auto& monkeyLine : monkeyLines) {
                std::getline(file, monkeyLine);
            }
            monkeys.back().setFromLines(monkeyLines);
        }
    }

    uint64_t commonModuloClass = 1;
    for(const auto& monkey : monkeys) {
        commonModuloClass *= monkey.testDivisor;
    }
    for(auto& monkey : monkeys) {
        monkey.operation.moduloClass = commonModuloClass;
    }
    std::cout << std::format("The common modulo class is {}.\n\n", commonModuloClass);
}

void KeepAwaySimulation::simulateRound() noexcept
{
    for(auto& monkey : monkeys) {
        while (not monkey.items.empty()) {
            const auto throwResult = monkey.handleNextItem();
            monkeys[throwResult.first].items.emplace_back(throwResult.second);
        }
    }
}

void KeepAwaySimulation::printInspectCounts() const noexcept
{
    for(size_t i = 0; i < monkeys.size(); ++i) {
        std::cout << std::format("Monkey {} inspected items {:5} times.\n", i, monkeys[i].itemInspectedCount);
    }
}

uint64_t KeepAwaySimulation::calculateMonkeyBusiness() const noexcept
{
    std::vector<const Monkey*> monkeysSorted(monkeys.size());
    for(size_t i = 0; i < monkeys.size(); ++i) {
        monkeysSorted[i] = &monkeys[i];
    }
    std::ranges::sort(monkeysSorted, std::greater{}, &Monkey::itemInspectedCount);

    return monkeysSorted[0]->itemInspectedCount * monkeysSorted[1]->itemInspectedCount;
}

int main()
{
    std::string_view filepath = "monkeyprediction_input";
    {
        ReliefDivisor = 3;
        KeepAwaySimulation keepAwaySimulation;
        keepAwaySimulation.parseStartState(filepath);
        for (int i = 0; i < 20; ++i) {
            keepAwaySimulation.simulateRound();
        }

        std::cout << std::format("The monkey business value is {}.\n", keepAwaySimulation.calculateMonkeyBusiness());
    }
    {
        std::cout << "\n\nPart 2:\n";
        ReliefDivisor = 1;
        std::cout << "Relief divisor set from 3 to 1\n";

        KeepAwaySimulation keepAwaySimulation;
        keepAwaySimulation.parseStartState(filepath);

//        std::vector<int> debugPrintRounds = { 1, 20, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000 };
        for (int i = 0; i < 10000; ++i) {
            keepAwaySimulation.simulateRound();

//            if (std::ranges::find(debugPrintRounds, i + 1) != debugPrintRounds.end()) {
//                std::cout << std::format("\n\n-- After round {} --\n", i + 1);
//                keepAwaySimulation.printInspectCounts();
//            }
        }

        std::cout << std::format("The monkey business value is {}.\n", keepAwaySimulation.calculateMonkeyBusiness());
    }

    return 0;
}
