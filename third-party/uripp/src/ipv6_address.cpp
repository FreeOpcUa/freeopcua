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
#include "ipv6_address.h"
#include "ip_address.h"
#include <sstream>
#include <stdexcept>
namespace uripp {
    const char ipv6_address::SEPARATOR_CHAR = ':';
    ipv6_address::ipv6_address() {
        hextets_[0] = 0;
        hextets_[1] = 0;
        hextets_[2] = 0;
        hextets_[3] = 0;
        hextets_[4] = 0;
        hextets_[5] = 0;
        hextets_[6] = 0;
        hextets_[7] = 0;
    }
    ipv6_address::ipv6_address(const std::string& v) {
        std::string::const_iterator first = v.begin();
        if (!parse(first, v.end(), *this) || first != v.end())
            throw std::invalid_argument("invalid IP v6 address: \"" + v + "\"");
    }
    bool ipv6_address::is_null() const {
        return !hextets_[0] && !hextets_[1] && !hextets_[2] && !hextets_[3] &&
            !hextets_[4] && !hextets_[5] && !hextets_[6] && !hextets_[7];
    }
    bool ipv6_address::zero_run(size_t& first, size_t& last) const {
        first = std::string::npos;
        size_t len = 0;
        size_t curi;
        size_t curn = 0;
        for (size_t i = 0;; ++i)
            if (i == 8 || hextets_[i]) { // Broke run.
                if (len < curn) { // New longest.
                    first = curi;
                    len = curn;
                }
                if (i == 8)
                    break;
                curn = 0;
            } else { // 0 hextet.
                if (!curn) // Start.
                    curi = i;
                ++curn;
            }
        if (!len) {
            last = first;
            return false;
        }
        last = first + len;
        return true;
    }
    std::string ipv6_address::string(bool compress) const {
        std::ostringstream oss;
        write(oss, compress);
        return oss.str();
    }
    std::ostream& ipv6_address::write(std::ostream& os, bool compress) const {
        size_t zfirst;
        size_t zlast;
        if (compress)
            zero_run(zfirst, zlast);
        else {
            zfirst = std::string::npos;
            zlast = std::string::npos;
        }
        for (size_t i = 0; i < 8; ++i)
            if (i < zfirst || zlast <= i) {
                if (i)
                    os << SEPARATOR_CHAR;
                os << std::hex << std::uppercase << hextets_[i];
            } else if (i == zfirst)
                os << SEPARATOR_CHAR;
        if (zlast == 8)
            os << SEPARATOR_CHAR;
        return os;
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, unsigned short& v) {
        std::string::const_iterator f = first;
        unsigned int tmp = 0;
        size_t i = 0;
        for (; i < 4; ++i, ++f) {
            if (f == last)
                break;
            unsigned int c = (unsigned int)*f;
            if ('0' <= c && c <= '9')
                tmp = (tmp << 4) + (c - '0');
            else if ('A' <= c && c <= 'F')
                tmp = (tmp << 4) + (c - 'A' + 10);
            else if ('a' <= c && c <= 'f')
                tmp = (tmp << 4) + (c - 'a' + 10);
            else
                break;
        }
        if (!i)
            return false;
        v = tmp;
        first = f;
        return true;
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, ipv6_address& v) {
        std::string::const_iterator f = first;
        ipv6_address ip;
        size_t elide;
        bool elided = false;
        size_t i = 0;
        std::string::const_iterator digits;
        for (; i < 8; ++i) {
            if (f == last)
                break;
            if (i && (*f != ipv6_address::SEPARATOR_CHAR || ++f == last)) // Need separator.
                break;
            if (*f == ipv6_address::SEPARATOR_CHAR) { // Elide.
                if (elided) // Cannot have 2 elides.
                    return false;
                ip.hextets_[i] = 0;
                elide = i;
                elided = true;
                if (!i)
                    ++f;
            } else {
                std::string::const_iterator anchor = f;
                if (!parse(f, last, ip.hextets_[i]))
                    break;
                if (i < 7 && f != last && *f == ip_address::SEPARATOR_CHAR) { // Might be v4.
                    ip_address v4;
                    if (parse(anchor, last, v4)) { // Got it.
                        ip.hextets_[i++] = ((unsigned int)v4.octets_[0] << 8) | v4.octets_[1];
                        ip.hextets_[i++] = ((unsigned int)v4.octets_[2] << 8) | v4.octets_[3];
                        f = anchor;
                        break;
                    }
                }
            }
        }
        if (i == 8) // At most 1 elided hextet, already zeroed.
            v = ip;
        else { // Need to insert zeros.
            if (!elided)
                return false;
            size_t j = 0;
            size_t zeros = 8 - i;
            for (i = 0; i < 8; ++i) {
                unsigned short u;
                if (elide < i && zeros) {
                    u = 0;
                    --zeros;
                } else
                    u = ip.hextets_[j++];
                v.hextets_[i] = u;
            }
        }
        first = f;
        return true;
    }
}
