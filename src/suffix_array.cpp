#include "suffix_array.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

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

std::vector<int> build_suffix_array(const std::string& text){
    return build_suffix_array_optimized(text);
}