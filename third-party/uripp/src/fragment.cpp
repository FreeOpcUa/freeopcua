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
#include "fragment.h"
#include "urisyn.h"
#include <stdexcept>
namespace uripp {
    fragment::fragment() {}
    fragment::fragment(const std::string& v) {
        if (!v.empty()) {
            std::string::const_iterator first = v.begin();
            if (!parse(first, v.end(), *this) || first != v.end())
                throw std::invalid_argument("invalid URI fragment: \"" + v + "\"");
        }
    }
    std::string fragment::encoding() const {
        return urisyn::encode(urisyn::FRAGMENT_TRAITS, string_);
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, fragment& v, std::string* errs) {
        std::string::const_iterator f = first;
        fragment tmp;
        std::string e;
        urisyn::parse(urisyn::FRAGMENT_TRAITS, f, last, tmp.string_);
        if (tmp.empty())
            return false;
        if (!urisyn::decode(tmp.string_)) { // Invalid encoding.
            if (!errs)
                return false;
            e = ", invalid encoding (using as is)";
        }
        if (errs && !e.empty()) {
            if (!errs->empty())
                *errs += "; ";
            *errs += "URI fragment: \"" + tmp.string_ + "\"" + e;
        }
        v = tmp;
        first = f;
        return true;
    }
}
