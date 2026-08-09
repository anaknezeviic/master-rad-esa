#include "esa.hpp"

#include "lcp.hpp"
#include "suffix_array.hpp"

EnhancedSuffixArray build_esa(const std::string& text)
{
    EnhancedSuffixArray esa;

    esa.text = text;

    esa.suffix_array = build_suffix_array(text);

    const int n = static_cast<int>(text.size());

    esa.inverse_suffix_array.resize(n);

    for (int i = 0; i < n; ++i) {
        esa.inverse_suffix_array[esa.suffix_array[i]] = i;
    }

    esa.lcp_array =
        build_lcp_array(text, esa.suffix_array);

    esa.bwt.resize(n);

    for (int i = 0; i < n; ++i) {

        const int suffix_position =
            esa.suffix_array[i];

        if (suffix_position == 0) {
            esa.bwt[i] = '$';
        } else {
            esa.bwt[i] = text[suffix_position - 1];
        }
    }

    return esa;
}