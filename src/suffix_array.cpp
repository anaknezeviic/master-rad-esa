#include "suffix_array.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

namespace {

bool is_lms_position(int position, const std::vector<unsigned char>& is_s_type) {
    return position > 0 &&
           is_s_type[position] &&
           !is_s_type[position - 1];
}

std::vector<unsigned char> classify_suffix_types(const std::vector<int>& text) {
    const int n = static_cast<int>(text.size());

    std::vector<unsigned char> is_s_type(n, 0);

    is_s_type[n - 1] = 1;

    for (int i = n - 2; i >= 0; --i) {
        if (text[i] < text[i + 1]) {
            is_s_type[i] = 1;
        } else if (text[i] > text[i + 1]) {
            is_s_type[i] = 0;
        } else {
            is_s_type[i] = is_s_type[i + 1];
        }
    }

    return is_s_type;
}

std::vector<int> get_bucket_sizes(const std::vector<int>& text, int alphabet_size) {
    std::vector<int> bucket_sizes(alphabet_size, 0);

    for (int character : text) {
        ++bucket_sizes[character];
    }

    return bucket_sizes;
}

std::vector<int> get_bucket_heads(const std::vector<int>& bucket_sizes) {
    std::vector<int> bucket_heads(bucket_sizes.size(), 0);

    int position = 0;

    for (std::size_t i = 0; i < bucket_sizes.size(); ++i) {
        bucket_heads[i] = position;
        position += bucket_sizes[i];
    }

    return bucket_heads;
}

std::vector<int> get_bucket_tails(const std::vector<int>& bucket_sizes) {
    std::vector<int> bucket_tails(bucket_sizes.size(), 0);

    int position = 0;

    for (std::size_t i = 0; i < bucket_sizes.size(); ++i) {
        position += bucket_sizes[i];
        bucket_tails[i] = position - 1;
    }

    return bucket_tails;
}

std::vector<int> induced_sort(const std::vector<int>& text, const std::vector<unsigned char>& is_s_type,
                              const std::vector<int>& lms_positions, int alphabet_size) {
    const int n = static_cast<int>(text.size());

    std::vector<int> suffix_array(n, -1);

    const std::vector<int> bucket_sizes = get_bucket_sizes(text, alphabet_size);

    std::vector<int> bucket_tails = get_bucket_tails(bucket_sizes);

    for (auto it = lms_positions.rbegin(); it != lms_positions.rend(); ++it) {
        const int position = *it;
        const int character = text[position];

        suffix_array[bucket_tails[character]] = position;
        --bucket_tails[character];
    }

    std::vector<int> bucket_heads = get_bucket_heads(bucket_sizes);

    for (int i = 0; i < n; ++i) {
        const int suffix_position = suffix_array[i];

        if (suffix_position <= 0) {
            continue;
        }

        const int previous_position = suffix_position - 1;

        if (!is_s_type[previous_position]) {
            const int character = text[previous_position];

            suffix_array[bucket_heads[character]] = previous_position;

            ++bucket_heads[character];
        }
    }

    bucket_tails = get_bucket_tails(bucket_sizes);

    for (int i = n - 1; i >= 0; --i) {
        const int suffix_position = suffix_array[i];

        if (suffix_position <= 0) {
            continue;
        }

        const int previous_position = suffix_position - 1;

        if (is_s_type[previous_position]) {
            const int character = text[previous_position];

            suffix_array[bucket_tails[character]] = previous_position;

            --bucket_tails[character];
        }
    }

    return suffix_array;
}

std::vector<int> get_lms_positions(const std::vector<unsigned char>& is_s_type) {
    std::vector<int> lms_positions;

    for (int i = 1; i < static_cast<int>(is_s_type.size()); ++i) {
        if (is_lms_position(i, is_s_type)) {
            lms_positions.push_back(i);
        }
    }

    return lms_positions;
}

bool lms_substrings_are_equal(const std::vector<int>& text, const std::vector<unsigned char>& is_s_type, int first, int second) {
    if (first == second) {
        return true;
    }

    int offset = 0;

    while (true) {
        const int first_position = first + offset;
        const int second_position = second + offset;

        if (text[first_position] != text[second_position]) {
            return false;
        }

        const bool first_is_lms = is_lms_position(first_position, is_s_type);

        const bool second_is_lms = is_lms_position(second_position, is_s_type);

        if (offset > 0 && first_is_lms && second_is_lms) {
            return true;
        }

        if (first_is_lms != second_is_lms) {
            return false;
        }

        ++offset;
    }
}

std::vector<int> build_reduced_string(const std::vector<int>& text, const std::vector<unsigned char>& is_s_type,
                                      const std::vector<int>& suffix_array, const std::vector<int>& lms_positions, 
                                      int& name_count) {
    const int n = static_cast<int>(text.size());

    std::vector<int> lms_name(n, -1);

    int current_name = -1;
    int previous_lms = -1;

    for (int suffix_position : suffix_array) {
        if (!is_lms_position(suffix_position, is_s_type)) {
            continue;
        }

        if (previous_lms == -1) {
            ++current_name;
        } else if (!lms_substrings_are_equal(text, is_s_type, previous_lms, suffix_position)) {
            ++current_name;
        }

        lms_name[suffix_position] = current_name;
        previous_lms = suffix_position;
    }

    name_count = current_name + 1;

    std::vector<int> reduced_string;
    reduced_string.reserve(lms_positions.size());

    for (int position : lms_positions) {
        reduced_string.push_back(lms_name[position]);
    }

    return reduced_string;
}

std::vector<int> sais(const std::vector<int>& text, int alphabet_size) {
    const int n = static_cast<int>(text.size());

    if (n == 1) {
        return {0};
    }

    const std::vector<unsigned char> is_s_type = classify_suffix_types(text);
    const std::vector<int> lms_positions = get_lms_positions(is_s_type);

    const std::vector<int> initial_suffix_array = induced_sort(text, is_s_type, lms_positions, alphabet_size);

    int name_count = 0;

    const std::vector<int> reduced_string = build_reduced_string(text, is_s_type, initial_suffix_array, lms_positions, name_count);

    std::vector<int> reduced_suffix_array(reduced_string.size());

    if (name_count == static_cast<int>(reduced_string.size())) {
        for (int i = 0; i < static_cast<int>(reduced_string.size()); ++i) {
            reduced_suffix_array[reduced_string[i]] = i;
        }
    } else {
        reduced_suffix_array = sais(reduced_string, name_count);
    }

    std::vector<int> sorted_lms_positions;
    sorted_lms_positions.reserve(lms_positions.size());

    for (int reduced_position : reduced_suffix_array) {
        sorted_lms_positions.push_back(lms_positions[reduced_position]);
    }

    return induced_sort(text, is_s_type, sorted_lms_positions, alphabet_size);
}
}

std::vector<int> build_suffix_array_baseline(const std::string& text){
    
    const int n = static_cast<int>(text.size());

    if (n == 0) {
        return {};
    }

    std::vector<int> suffix_array(n);
    std::vector<int> equivalence_class(n);
    std::vector<int> new_equivalence_class(n);

    std::iota(suffix_array.begin(), suffix_array.end(), 0);

    for (int i = 0; i < n; ++i) {
        equivalence_class[i] = static_cast<unsigned char>(text[i]);
    }

    for (int k = 1; k < n; k *= 2) {

        auto compare_suffixes = [&](int i, int j){
            
            if (equivalence_class[i] != equivalence_class[j]){
                return equivalence_class[i] < equivalence_class[j];
            }
            const int equivalence_class_i_second = (i + k < n) ? equivalence_class[i + k] : -1;
            const int equivalence_class_j_second = (j + k < n) ? equivalence_class[j + k] : -1;

            return equivalence_class_i_second < equivalence_class_j_second;
        };

        std::sort(suffix_array.begin(), suffix_array.end(), compare_suffixes);

        new_equivalence_class[ suffix_array[0] ] = 0;

        for (int i = 1; i < n; ++i) {

            const int previous = suffix_array[i - 1];
            const int current = suffix_array[i];
            new_equivalence_class[current] = new_equivalence_class[previous] + (compare_suffixes(previous, current) ? 1 : 0);
        }

        equivalence_class = new_equivalence_class;

        if (equivalence_class[ suffix_array[n - 1] ] == n - 1) {
            break;
        }
    }

    return suffix_array;
}

std::vector<int> build_suffix_array_optimized(const std::string& text){
    const int n = static_cast<int>(text.size());

    if (n == 0) {
        return {};
    }

    std::vector<int> suffix_array(n);
    std::vector<int> equivalence_class(n);

    constexpr int alphabet_size = 256;

    std::vector<int> count(alphabet_size, 0);

    for (unsigned char character : text){
        ++count[character];
    }

    for (int i = 1; i < alphabet_size; ++i){
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; --i){
        const unsigned char character = static_cast<unsigned char>(text[i]);

        suffix_array[--count[character]] = i;
    }

    equivalence_class[suffix_array[0]] = 0;

    int class_count = 1;

    for (int i = 1; i < n; ++i){
        if (text[suffix_array[i]] != text[suffix_array[i - 1]]) {
            ++class_count;
        }

        equivalence_class[suffix_array[i]] = class_count - 1;
    }

    std::vector<int> second_key_order(n);
    std::vector<int> new_suffix_array(n);
    std::vector<int> new_equivalence_class(n);

    for (int k = 1; k < n && class_count < n; k *= 2){
        int index = 0;

        for (int i = n - k; i < n; ++i) {
            if (i >= 0) {
                second_key_order[index++] = i;
            }
        }

    
        for (int i = 0; i < n; ++i) {
            if (suffix_array[i] >= k) {
                second_key_order[index++] = suffix_array[i] - k;
            }
        }

        std::vector<int> class_size(class_count, 0);

        for (int position : second_key_order){
            ++class_size[equivalence_class[position]];
        }

        std::vector<int> class_start(class_count, 0);

        for (int i = 1; i < class_count; ++i){
            class_start[i] = class_start[i - 1] + class_size[i - 1];
        }

        for (int position : second_key_order){
            const int current_class = equivalence_class[position];

            new_suffix_array[class_start[current_class]++] = position;
        }

        new_equivalence_class[new_suffix_array[0]] = 0;

        int new_class_count = 1;

        for (int i = 1; i < n; ++i){

            const int previous = new_suffix_array[i - 1];
            const int current = new_suffix_array[i];
            const int previous_first = equivalence_class[previous];
            const int current_first = equivalence_class[current];
            const int previous_second = (previous + k < n) ? equivalence_class[previous + k] : -1;
            const int current_second =(current + k < n)? equivalence_class[current + k] : -1;

            if (previous_first != current_first || previous_second != current_second){
                ++new_class_count;
            }

            new_equivalence_class[current] = new_class_count - 1;
        }

        suffix_array.swap(new_suffix_array);

        equivalence_class.swap(new_equivalence_class);

        class_count = new_class_count;

        if (k > n / 2) {
            break;
        }
    }

    return suffix_array;
}

std::vector<int> build_suffix_array_sais(const std::string& text) {
    const int n = static_cast<int>(text.size());

    if (n == 0) {
        return {};
    }

    std::vector<int> encoded_text;
    encoded_text.reserve(n + 1);

    for (unsigned char character : text) {
        encoded_text.push_back(static_cast<int>(character) + 1);
    }

    encoded_text.push_back(0);
    constexpr int alphabet_size = 257;

    const std::vector<int> suffix_array_with_sentinel = sais(encoded_text, alphabet_size);

    std::vector<int> suffix_array;
    suffix_array.reserve(n);

    for (int position : suffix_array_with_sentinel) {
        if (position != n) {
            suffix_array.push_back(position);
        }
    }

    return suffix_array;
}

std::vector<int> build_suffix_array(const std::string& text){
    return build_suffix_array_sais(text);
}