/*
Copyright (c) 2009 zooml.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "query.h"
#include "urisyn.h"
#include <algorithm>
#include <stdexcept>
namespace uripp {
    const char query::PAIRS_SEP_CHAR = '&';
    const char query::KEY_VALUE_SEP_CHAR = '=';
    query::query() : sorted_(false) {}
    query::query(const std::string& v, bool dosort) : sorted_(false) {
        std::string::const_iterator first = v.begin();
        std::string errs;
        if (!parse(first, v.end(), *this, &errs) || first != v.end())
            throw std::invalid_argument("invalid URI query: \"" + v + "\"");
        if (!errs.empty())
            throw std::invalid_argument(errs);
        if (!empty() && dosort)
            sort();
    }
    void query::sort() {
        std::sort(begin(), end());
        sorted_ = true;
    }
    std::string query::encoding() const {
        std::string s;
        for (const_iterator it = begin(); it != end(); ++it) {
            if (it != begin())
                s += PAIRS_SEP_CHAR;
            s += urisyn::encode(urisyn::QUERY_TRAITS, it->first); // Encode and append.
            s += KEY_VALUE_SEP_CHAR;
            s += urisyn::encode(urisyn::QUERY_TRAITS, it->second); // Encode and append.
        }
        return s;
    }
    query::const_iterator query::find(const std::string& key) const {
        // DUPLICATED BELOW!!!
        const_iterator it;
        if (sorted_) {
            const_iterator lb = std::lower_bound(begin(), end(), value_type(key, ""));
            it = (lb != end() && lb->first == key) ? lb : end();
        } else {
            for (it = begin(); it != end(); ++it)
                if (it->first == key)
                    break;
        }
        return it;
    }
    query::iterator query::find(const std::string& key) {
        // DUPLICATED ABOVE!!!
        iterator it;
        if (sorted_) {
            iterator lb = std::lower_bound(begin(), end(), value_type(key, ""));
            it = (lb != end() && lb->first == key) ? lb : end();
        } else {
            for (it = begin(); it != end(); ++it)
                if (it->first == key)
                    break;
        }
        return it;
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, query& v, std::string* errs) {
        std::string::const_iterator f = first;
        query tmp;
        std::string e;
        for (bool eof = false; !eof;) {
            std::string key;
            std::string value;
            bool nokvsep = false;
            for (int i = 0; i < 2; ++i) {
                std::string::const_iterator anchor = f;
                // Read until end or separator.
                for (;; ++f)
                    if (f == last) {
                        eof = true;
                        break;
                    } else {
                        char c = *f;
                        if (c == query::PAIRS_SEP_CHAR || (!i && c == query::KEY_VALUE_SEP_CHAR))
                            break;
                        if (urisyn::QUERY_TRAITS.char_class[(unsigned char)c] < urisyn::CVAL) {
                            eof = true;
                            break;
                        }
                    }
                if (i)
                    value.assign(anchor, f);
                else {
                    key.assign(anchor, f);
                    if (eof || *f == query::PAIRS_SEP_CHAR) { // No kv sep or value.
                        i = 2;
                        nokvsep = true;
                    }
                }
                if (!eof)
                    ++f; // Skip separator.
            }
            if (key.empty()) { // Need a key but missing.
                if (!value.empty()) {
                    if (!errs || tmp.empty())
                        return false;
                    e += ", (" + convert(f - first) + ") missing key (ignoring value)";
                }
            } else if (nokvsep) { // Missing '='.
                if (!errs || tmp.empty())
                    return false;
                e += ", (" + convert(f - first) + ") missing \"" + std::string(1, query::KEY_VALUE_SEP_CHAR) + "\" (ignoring key)";
            } else {
                if (!urisyn::decode(key)) { // Bad encoding.
                    if (!errs || tmp.empty())
                        return false;
                    e += ", (" + convert(f - first) + ") invalid key encoding (using as is)";
                }
                if (!urisyn::decode(value)) { // Bad encoding.
                    if (!errs || tmp.empty())
                        return false;
                    e += ", (" + convert(f - first) + ") invalid value encoding (using as is)";
                }
                tmp.push_back(std::make_pair(key, value));
            }
        }
        if (errs && !e.empty()) {
            if (!errs->empty())
                *errs += "; ";
            *errs += "URI query: \"" + std::string(first, f) + "\"" + e;
        }
        v = tmp;
        first = f;
        return true;
    }
}
