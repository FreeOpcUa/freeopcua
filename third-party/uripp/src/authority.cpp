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
#include "authority.h"
#include "ip_address.h"
#include "ipv6_address.h"
#include "domain_name.h"
#include <sstream>
#include <stdexcept>
namespace uripp {
    const char authority::IP_LITERAL_BEGIN_CHAR = '[';
    const char authority::IP_LITERAL_END_CHAR = ']';
    const char authority::PORT_SEPARATOR_CHAR = ':';
    authority::authority() : port_(0) {}
    authority::authority(const std::string& v) : port_(0) {
        std::string::const_iterator first = v.begin();
        if (!parse(first, v.end(), *this) || first != v.end())
            throw std::invalid_argument("invalid URI authority: \"" + v + "\"");
    }
    std::string authority::string() const {
        std::ostringstream oss;
        operator <<(oss);
        return oss.str();
    }
    std::ostream& authority::operator <<(std::ostream& os) const {
        os << host_;
        if (port_)
            os << PORT_SEPARATOR_CHAR << port_;
        return os;
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, authority& v) {
        std::string::const_iterator f = first;
        authority::host_type_e e;
        // Just parse to get the end.
        // Note string is not put in canonical form.
        ip_address ipaddr;
        if (parse(f, last, ipaddr)) // IP address.
            e = authority::IP_ADDRESS;
        else {
            domain_name dn;
            if (parse(f, last, dn)) // Domain name.
                e = authority::DOMAIN_NAME;
            else {
                if (f == last || *f != authority::IP_LITERAL_BEGIN_CHAR)
                    return false;
                ++f;
                ipv6_address ipv6addr;
                if (!parse(f, last, ipv6addr)) // Not IP v6 address.
                    return false;
                if (f == last || *f != authority::IP_LITERAL_END_CHAR)
                    return false;
                e = authority::IPV6_ADDRESS;
                ++f;
            }
        }
        std::string host(first, f);
        unsigned int port = 0;
        if (f != last && *f == authority::PORT_SEPARATOR_CHAR) {
            ++f;
            std::string::const_iterator anchor = f;
            for (; f != last; ++f) {
                char c = *f;
                if (c < '0' || '9' < c)
                    break;
                else {
                    port = port * 10 + (c - '0');
                    if (65535 < port)
                        return false; // Port out of range.
                }
            }
            if (f == anchor)
                return false; // Separator but no digits.
        }
        v.host_type_ = e;
        v.host_ = host;
        v.port_ = port;
        first = f;
        return true;
    }
}
