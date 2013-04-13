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
#include "uri.h"
#include "urisyn.h"
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <stdexcept>
namespace uripp {
    uri::uri() {}
    uri::uri(const std::string& v) {
        std::string::const_iterator first = v.begin();
        if (!parse(first, v.end(), *this) || first != v.end())
            throw std::invalid_argument("invalid URI: \"" + v + "\"");
    }
    bool uri::is_null() const {
        return scheme_.is_null() && authority_.is_null() && path_.empty() && query_.empty() && fragment_.empty();
    }
    std::string uri::encoding() const {
        std::ostringstream oss;
        operator <<(oss);
        return oss.str();
    }
    std::ostream& uri::operator <<(std::ostream& os) const {
        if (!scheme_.is_null())
            os << scheme_ << urisyn::SCHEME_TRAITS.end_char;
        if (!authority_.is_null())
            os << urisyn::AUTHORITY_TRAITS.begin_cstring << authority_;
        os << path_;
        if (!query_.empty())
            os << urisyn::QUERY_TRAITS.begin_char << query_;
        if (!fragment_.empty())
            os << urisyn::FRAGMENT_TRAITS.begin_char << fragment_;
        return os;
    }
    bool uri::parse_literal(std::string::const_iterator& first, std::string::const_iterator last, const char* v) {
        std::string::const_iterator f = first;
        if (!v)
            return false;
        for (; *v;) {
            if (f == last)
                return false;
            if (*f++ != *v++)
                return false;
        }
        first = f;
        return true;
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, uri& v, std::string* errs) {
        std::string::const_iterator f = first;
        char endc;
        uri tmp;
        std::string e;
        // Parse scheme, if any.
        if (parse(f, last, tmp.scheme_, &endc) && !endc) {
            // Scheme name found but no ":": relative, so undo scheme parse.
            f = first;
            tmp.scheme_ = scheme();
        }
        // Parse authority, if any.
        std::string::const_iterator anchor = f;
        if (uri::parse_literal(f, last, urisyn::AUTHORITY_TRAITS.begin_cstring)) {
            if (!parse(f, last, tmp.authority_))
                f = anchor; // Not authority (must be empty path segment instead).
        } else
            f = anchor;
        // Parse path, if any.
        parse(f, last, tmp.path_, errs ? &e : 0);
        // Parse query, if any.
        if (f != last && *f == urisyn::QUERY_TRAITS.begin_char) {
            ++f;
            parse(f, last, tmp.query_, errs ? &e : 0);
        }
        // Parse fragment, if any.
        if (f != last && *f == urisyn::FRAGMENT_TRAITS.begin_char) {
            ++f;
            parse(f, last, tmp.fragment_, errs ? &e : 0);
        }
        if (tmp.is_null())
            return false;
        if (errs && !e.empty()) {
            if (!errs->empty())
                *errs += "; ";
            *errs += "URI \"" + std::string(first, f) + "\": " + e;
        }
        v = tmp;
        first = f;
        return true;
    }
}
