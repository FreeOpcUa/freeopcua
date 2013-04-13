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
#include "domain_name.h"
#include <boost/algorithm/string.hpp>
namespace uripp {
    const char domain_name::SEPARATOR_CHAR = '.';
    const char domain_name::char_class[] = {
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,2,0,0,
        1,1,1,1,1,1,1,1, 1,1,0,0,0,0,0,0,
        0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1, 1,1,1,0,0,0,0,0,
        0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1, 1,1,1,0,0,0,0,0, // 7F
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    };
    domain_name::domain_name() {}
    domain_name::domain_name(const std::string& v) {
        std::string::const_iterator first = v.begin();
        if (!parse(first, v.end(), *this) || first != v.end())
            throw std::invalid_argument("invalid domain name: \"" + v + "\"");
    }
    domain_name& domain_name::operator +=(const domain_name& rhs) {
        if (is_null())
            string_ = rhs.string_;
        else if (!rhs.is_null()) {
            string_.append(1, SEPARATOR_CHAR);
            string_.append(rhs.string_);
        }
        return *this;
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, domain_name& v) {
        std::string::const_iterator f = first;
        int state = 0; // 0=1st char
        std::string::const_iterator prev_cls_1 = last;
        for (; f != last; ++f) {
            char c = *f;
            char cls = domain_name::char_class[(unsigned char)c];
            if (state) { // 2nd or higher char in label.
                if (!cls) { // Not valid.
                    if (prev_cls_1 + 1 != f) // Prev must be class 1.
                        break;
                    if (c != domain_name::SEPARATOR_CHAR)
                        break;
                    state = 0;
                } else if (cls == 1)
                    prev_cls_1 = f;
            } else { // 1st char in label.
                if (cls != 1) // Must be class 1.
                    break;
                state = 1;
                prev_cls_1 = f;
            }
        }
        if (f == first)
            return false;
        f = prev_cls_1 + 1; // Always stop at last class 1.
        v.string_.assign(first, f);
        boost::to_lower(v.string_);
        first = f;
        return true;
    }
}
