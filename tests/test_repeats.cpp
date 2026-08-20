#include "esa.hpp"
#include "repeats.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>


bool contains_repeat(
    const std::vector<Repeat>& repeats,
    const std::string& sequence,
    std::vector<int> expected_positions
) {
    std::sort(
        expected_positions.begin(),
        expected_positions.end()
    );

    for (const Repeat& repeat : repeats) {
        if (repeat.sequence != sequence) {
            continue;
        }

        std::vector<int> actual_positions =
            repeat.positions;

        std::sort(
            actual_positions.begin(),
            actual_positions.end()
        );

        return actual_positions == expected_positions;
    }

    return false;
}


bool test_published_repeat_example() {
    const std::string text =
        "NLAREPLNOREPTFCGIREPTLSIG";

    RepeatOptions options;
    options.min_length = 3;
    options.type = RepeatType::Both;

    const EnhancedSuffixArray esa =
        build_esa(text);

    const auto maximal =
        find_maximal_repeats(
            esa,
            options
        );

    const auto supermaximal =
        find_supermaximal_repeats(
            esa,
            options
        );

    if (maximal.size() != 2) {
        std::cerr
            << "Published example: expected 2 maximal "
            << "repeats, got "
            << maximal.size()
            << '\n';

        return false;
    }

    if (!contains_repeat(
            maximal,
            "REP",
            {3, 9, 17}
        )) {

        std::cerr
            << "Published example: REP is missing "
            << "or has incorrect positions.\n";

        return false;
    }

    if (!contains_repeat(
            maximal,
            "REPT",
            {9, 17}
        )) {

        std::cerr
            << "Published example: REPT is missing "
            << "or has incorrect positions.\n";

        return false;
    }

    if (supermaximal.size() != 1) {
        std::cerr
            << "Published example: expected 1 "
            << "supermaximal repeat, got "
            << supermaximal.size()
            << '\n';

        return false;
    }

    if (!contains_repeat(
            supermaximal,
            "REPT",
            {9, 17}
        )) {

        std::cerr
            << "Published example: supermaximal "
            << "REPT is missing.\n";

        return false;
    }

    return true;
}


bool test_dna_repeat_example() {
    const std::string text =
        "ACGTACGTGATTACANN";

    RepeatOptions options;
    options.min_length = 2;
    options.type = RepeatType::Both;

    const EnhancedSuffixArray esa =
        build_esa(text);

    const auto maximal =
        find_maximal_repeats(
            esa,
            options
        );

    const auto supermaximal =
        find_supermaximal_repeats(
            esa,
            options
        );

    if (maximal.size() != 3) {
        std::cerr
            << "DNA example: expected 3 maximal "
            << "repeats, got "
            << maximal.size()
            << '\n';

        return false;
    }

    if (!contains_repeat(
            maximal,
            "ACGT",
            {0, 4}
        )) {

        std::cerr
            << "DNA example: ACGT is incorrect.\n";

        return false;
    }

    if (!contains_repeat(
            maximal,
            "AC",
            {0, 4, 12}
        )) {

        std::cerr
            << "DNA example: AC is incorrect.\n";

        return false;
    }

    if (!contains_repeat(
            maximal,
            "TAC",
            {3, 11}
        )) {

        std::cerr
            << "DNA example: TAC is incorrect.\n";

        return false;
    }

    if (supermaximal.size() != 2) {
        std::cerr
            << "DNA example: expected 2 supermaximal "
            << "repeats, got "
            << supermaximal.size()
            << '\n';

        return false;
    }

    if (!contains_repeat(
            supermaximal,
            "ACGT",
            {0, 4}
        )) {

        std::cerr
            << "DNA example: supermaximal ACGT "
            << "is incorrect.\n";

        return false;
    }

    if (!contains_repeat(
            supermaximal,
            "TAC",
            {3, 11}
        )) {

        std::cerr
            << "DNA example: supermaximal TAC "
            << "is incorrect.\n";

        return false;
    }

    return true;
}


int main() {
    bool success = true;

    if (!test_published_repeat_example()) {
        success = false;
    }
    else {
        std::cout
            << "Published repeat example passed.\n";
    }

    if (!test_dna_repeat_example()) {
        success = false;
    }
    else {
        std::cout
            << "DNA repeat regression test passed.\n";
    }

    if (!success) {
        return 1;
    }

    std::cout
        << "All repeat tests passed.\n";

    return 0;
}