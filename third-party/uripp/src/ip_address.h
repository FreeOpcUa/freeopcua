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
#ifndef uripp_ip_address_h
#define uripp_ip_address_h
#include "apidefs.h"
#include <string>
#include <iostream>
namespace uripp {
    class ipv6_address;
    /** \brief IP address.
     *
     * Syntax: Note that leading 0's are not valid.
     * <pre>
     * IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
     * dec-octet   = DIGIT                 ; 0-9
     *             | %%x31-39 DIGIT         ; 10-99
     *             | "1" 2DIGIT            ; 100-199
     *             | "2" %%x30-34 DIGIT     ; 200-249
     *             | "25" %%x30-35          ; 250-255
     * </pre>
     * @see http://tools.ietf.org/html/rfc3986#section-3.2.2 */
    class URIPP_API ip_address {
    public:
        ip_address(); ///< Construct null.
        /// Construct from string. Note that the string cannot
        /// be empty, it must contain valid chars.
        /// @exception std::invalid_argument if invalid
        ip_address(const std::string& v);
        /// Test if null (all 0's).
        bool is_null() const {return !octets_[0] && !octets_[1] && !octets_[2] && !octets_[3];}
        std::string string() const; ///< Calculate string.
        std::ostream& operator <<(std::ostream& os) const; ///< Stream out.
        static const char SEPARATOR_CHAR; ///< separator ('.')
    private:
        friend bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, ip_address& v);
        friend bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, ipv6_address& v);
        unsigned char octets_[4];
    };
    /// Stream out IP address.
    inline std::ostream& operator <<(std::ostream& os, const ip_address& v) {return v.operator <<(os);}
    /// Parse IP address, returning whether found or not
    /// and advancing first and setting address if found.
    /// Does not skip leading space.
    bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, ip_address& v);
}
#endif
