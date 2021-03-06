/*
 * Levenshtein distance using the classic DP algorithm.
 *
 * Copyright 2013 Netherlands eScience Center
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef levenshtein_impl_h
#define levenshtein_impl_h

#include <set>
#include <vector>
#include <climits>
#include <algorithm>
#include <stdexcept>

const bool BLACKLIST = false;
const bool WHITELIST = true;
const char* const BLACKLIST_CHARS = ";:,'";
const char* const WHITELIST_CHARS = "ABCDEFGHIJKLMNOPQRSTUVXYZabcdefghijklmnopqrstuvxyz1234567890&=/_-?. ";
const int PENALTY = 2; 


static unsigned min(unsigned a, unsigned b, unsigned c)
{
    return std::min(a, std::min(b, c));
}

template <typename Char>
static unsigned levenshtein(Char const *a, size_t m, Char const *b, size_t n)
{

    std::set<char> chars;

    if (BLACKLIST == true) {
        char const * weight_chars = BLACKLIST_CHARS;
        chars.insert(weight_chars, weight_chars + strlen(weight_chars));
    }
    
    if (WHITELIST == true) {
        char const * weight_chars = WHITELIST_CHARS;
        chars.insert(weight_chars, weight_chars + strlen(weight_chars));
    }

    // Swap a and b if necessary to ensure m <= n.
    if (m > n) {
        std::swap(a, b);
        std::swap(m, n);
    }

    // Skip common prefix.
    while (m > 0 && n > 0 && *a == *b) {
        a++, b++;
        m--, n--;
    }

    
    while (m > 0 && n > 0 && a[m - 1] == b[n - 1]) {
        m--, n--;
    }

    // To save memory, we use unsigned as the type in the DP table.
    if (m > UINT_MAX || n > UINT_MAX) {
        throw std::length_error("string too long in Levenshtein distance.");
    }

    if (m == 0) {
        return n;
    }
    if (n == 0) {
        return m;
    }

    std::vector<unsigned> tab((m + 1) * 2);
    unsigned *cur = tab.data(), *prev = tab.data() + m + 1;

    for (size_t i = 0; i <= m; i++) {
        cur[i] = i;
    }

    for (size_t j = 1; j <= n; j++) {
        std::swap(cur, prev);
        cur[0] = j;

        for (size_t i = 1; i <= m; i++) {
            size_t w = 1;

            if (BLACKLIST == true) {
                if ( (chars.find(a[i - 1]) != chars.end()) || (chars.find(b[j - 1]) != chars.end()) ) {
                    w = PENALTY;
                }
            }

            if (WHITELIST == true) {
                if ( (chars.find(a[i - 1]) == chars.end()) || (chars.find(b[j - 1]) == chars.end()) ) {
                    w = PENALTY;
                }
            }
            

            if (a[i - 1] == b[j - 1]) {
                cur[i] = prev[i - 1];
            }
            else {
                cur[i] = min(cur[i - 1], prev[i], prev[i - 1]) + w;
            }
        }
    }

    return cur[m];
}

#endif  // levenshtein_impl_h
