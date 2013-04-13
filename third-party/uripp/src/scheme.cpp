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
#include "scheme.h"
#include "urisyn.h"
#include <stdexcept>
#include <boost/algorithm/string.hpp>
namespace uripp {
    scheme::scheme() {}
    scheme::scheme(const std::string& v) {
        std::string::const_iterator first = v.begin();
        char endc;
        if (!parse(first, v.end(), *this, &endc) || first != v.end() || endc)
            throw std::invalid_argument("invalid URI scheme: \"" + v + "\"");
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, scheme& v, char* endc) {
        //????? does endc need to be a ptr????
        std::string tmp;
        char ec;
        //????? should "not empty" requirement be put in traits????
        if (!urisyn::parse(urisyn::SCHEME_TRAITS, first, last, tmp, &ec) || tmp.empty())
            return false;
        boost::to_lower(tmp);
        v.string_ = tmp;
        if (endc)
            *endc = ec;
        return true;
    }
}
