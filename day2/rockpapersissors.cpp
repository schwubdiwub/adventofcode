#include <cassert>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

enum class Play : int8_t {
    Rock = 0,
    Paper = 1,
    Sissors = 2
};

uint64_t playToScore(Play play) noexcept;

enum class RoundOutcome {
    Win,
    Loss,
    Draw
};

RoundOutcome lhsOutcome(Play lhsPlay, Play rhsPlay) noexcept;
uint64_t roundOutcomeToScore(RoundOutcome outcome) noexcept;

struct Round {
    Play opponentPlay;
    Play myPlay;

    [[nodiscard]] uint64_t score() const noexcept
    {
        const auto playScore = playToScore(myPlay);
        const auto outcomeScore = roundOutcomeToScore(lhsOutcome(myPlay, opponentPlay));
        return playScore + outcomeScore;
    }
};

uint64_t playToScore(Play play) noexcept
{
    switch (play) {
        case Play::Rock:
            return 1;
        case Play::Paper:
            return 2;
        case Play::Sissors:
            return 3;
    }

    assert(false && "Invalid play input.");
    return 0;
}

RoundOutcome lhsOutcome(Play lhsPlay, Play rhsPlay) noexcept
{
    if(lhsPlay == rhsPlay) {
        return RoundOutcome::Draw;
    }

    const auto lhsInt = static_cast<int8_t>(lhsPlay);
    const auto rhsInt = static_cast<int8_t>(rhsPlay);

    if(lhsInt < rhsInt) {
        const auto difference = rhsInt - lhsInt;
        return difference == 1 ? RoundOutcome::Loss : RoundOutcome::Win;
    }
    else {
        assert(lhsInt > rhsInt);
        const auto difference = lhsInt - rhsInt;
        return difference == 2 ? RoundOutcome::Loss : RoundOutcome::Win;
    }

    assert(false && "Invalid play input.");
    return {};
}

uint64_t roundOutcomeToScore(RoundOutcome outcome) noexcept
{
    switch (outcome) {
        case RoundOutcome::Win:
            return 6;
        case RoundOutcome::Loss:
            return 0;
        case RoundOutcome::Draw:
            return 3;
    }

    assert(false && "Invalid round outcome input.");
    return -1;
}

Play opponentStrToPlay(const std::string& opponentStr) noexcept
{
    assert(opponentStr.size() == 1);
    switch (opponentStr[0]) {
        case 'A':
            return Play::Rock;
        case 'B':
            return Play::Paper;
        case 'C':
            return Play::Sissors;
        default:
            abort();
    }
}

Play myStrToPlay(const std::string& myStr) noexcept
{
    assert(myStr.size() == 1);
    switch (myStr[0]) {
        case 'X':
            return Play::Rock;
        case 'Y':
            return Play::Paper;
        case 'Z':
            return Play::Sissors;
        default:
            abort();
    }
}

RoundOutcome outcomeStrToOutcome(const std::string& outcomeStr) noexcept
{
    assert(outcomeStr.size() == 1);
    switch (outcomeStr[0]) {
        case 'X':
            return RoundOutcome::Loss;
        case 'Y':
            return RoundOutcome::Draw;
        case 'Z':
            return RoundOutcome::Win;
        default:
            abort();
    }
}

Play opponentPlayAndOutcomeToPlay(Play opponentPlay, RoundOutcome outcome) noexcept
{
    const auto opponentPlayInt = static_cast<int8_t>(opponentPlay);
    
    switch (outcome) {
        case RoundOutcome::Draw:
            return opponentPlay;
        case RoundOutcome::Loss: {
            const auto minusOne = opponentPlayInt - 1;
            return minusOne >= 0 ? static_cast<Play>(minusOne) : Play::Sissors;
        }
        case RoundOutcome::Win: {
            const auto plusOne = opponentPlayInt + 1;
            return plusOne <= 2 ? static_cast<Play>(plusOne) : Play::Rock;
        }
    }

    assert(false && "Invalid outcome input.");
    return {};
}

std::vector<Round> parseRounds(std::string_view filepath) noexcept
{
    const std::regex roundRegex("([A-C]) ([X-Z])");
    std::smatch playMatch;

    std::vector<Round> rounds;
    rounds.reserve(1024);

    std::fstream file(filepath.data());
    std::string line;
    while(std::getline(file, line)) {
        if(std::regex_match(line, playMatch, roundRegex)) [[likely]] {
            assert(playMatch.size() == 3);
            rounds.emplace_back(opponentStrToPlay(playMatch[1].str()), myStrToPlay(playMatch[2].str()));
        }
        else {
            assert(false && "Didn't expect no match.");
        }
    }

    return rounds;
}

std::vector<Round> parseRoundsPart2(std::string_view filepath) noexcept
{
    const std::regex roundRegex("([A-C]) ([X-Z])");
    std::smatch playMatch;

    std::vector<Round> rounds;
    rounds.reserve(1024);

    std::fstream file(filepath.data());
    std::string line;
    while(std::getline(file, line)) {
        if(std::regex_match(line, playMatch, roundRegex)) {
            assert(playMatch.size() == 3);
            const auto opponentPlay = opponentStrToPlay(playMatch[1].str());
            const auto desiredOutcome = outcomeStrToOutcome(playMatch[2].str());
            rounds.emplace_back(opponentPlay, opponentPlayAndOutcomeToPlay(opponentPlay, desiredOutcome));
        }
        else {
            assert(false && "Didn't expect no match.");
        }
    }

    return rounds;
}

int main()
{
    const auto rounds = parseRounds("rockpapersissors_input.txt");
    const auto myScore = std::accumulate(rounds.begin(), rounds.end(), 0ULL, [](uint64_t v, const Round& r) { return v + r.score(); });

    std::cout << std::format("My score after playing {} rounds following the strategy guide: {}\n", rounds.size(), myScore);



    std::cout << "Part2:\n";

    const auto rounds2 = parseRoundsPart2("rockpapersissors_input.txt");
    const auto myScore2 = std::accumulate(rounds2.begin(), rounds2.end(), 0ULL, [](uint64_t v, const Round& r) { return v + r.score(); });

    std::cout << std::format("My score after playing {} rounds following the strategy guide: {}\n", rounds2.size(), myScore2);


    return 0;
}
