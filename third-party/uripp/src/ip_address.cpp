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
#include "ip_address.h"
#include <sstream>
#include <stdexcept>
namespace uripp {
    const char ip_address::SEPARATOR_CHAR = '.';
    ip_address::ip_address() {
        octets_[0] = 0;
        octets_[1] = 0;
        octets_[2] = 0;
        octets_[3] = 0;
    }
    ip_address::ip_address(const std::string& v) {
        std::string::const_iterator first = v.begin();
        if (!parse(first, v.end(), *this) || first != v.end())
            throw std::invalid_argument("invalid IP address: \"" + v + "\"");
    }
    std::string ip_address::string() const {
        std::ostringstream oss;
        operator <<(oss);
        return oss.str();
    }
    std::ostream& ip_address::operator <<(std::ostream& os) const {
        return os << (int)octets_[0] << SEPARATOR_CHAR << (int)octets_[1] << SEPARATOR_CHAR << (int)octets_[2] << SEPARATOR_CHAR << (int)octets_[3];
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, ip_address& v) {
        std::string::const_iterator f = first;
        ip_address ip;
        for (size_t i = 0; i < 4; ++i) {
            if (f == last)
                return false;
            if (i && (*f++ != ip_address::SEPARATOR_CHAR || f == last))
                return false;
            int n;
            unsigned int c = (unsigned int)*f;
            if (c == '0') {
                n = 0;
                ++f;
            } else {
                if (c < '1' || '9' < c) // 1st dig.
                    return false;
                n = c - '0';
                if (++f != last) {
                    c = (unsigned int)*f;
                    if ('0' <= c && c <= '9') { // 2nd dig.
                        n = n * 10 + (c - '0');
                        if (++f != last) {
                            c = (unsigned int)*f;
                            if ('0' <= c && c <= '9') { // 3rd dig.
                                n = n * 10 + (c - '0');
                                if (255 < n)
                                    return false;
                                ++f;
                            }
                        }
                    }
                }
            }
            ip.octets_[i] = n;
        }
        v = ip;
        first = f;
        return true;
    }
}
