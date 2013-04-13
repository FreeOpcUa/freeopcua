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
#ifndef uripp_ipv6_address_h
#define uripp_ipv6_address_h
#include "apidefs.h"
#include <string>
#include <iostream>
namespace uripp {
    /* \brief IP v6 address.
     *
     * Syntax: Note that leading 0's are not valid.
     * <pre>
     * IPv6address =                            6( h16 ":" ) ls32
     *             |                       "::" 5( h16 ":" ) ls32
     *             | [               h16 ] "::" 4( h16 ":" ) ls32
     *             | [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
     *             | [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
     *             | [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
     *             | [ *4( h16 ":" ) h16 ] "::"              ls32
     *             | [ *5( h16 ":" ) h16 ] "::"              h16
     *             | [ *6( h16 ":" ) h16 ] "::"
     * ls32        = ( h16 ":" h16 ) | IPv4address
     *             ; least-significant 32 bits of address
     * h16         = 1*4HEXDIG
     *             ; 16 bits of address represented in hexadecimal
     * </pre>
     * @see http://tools.ietf.org/html/rfc3986#section-3.2.2 */
    class URIPP_API ipv6_address {
    public:
        ipv6_address(); ///< Construct null.
        /// Construct from string. Note that the string cannot
        /// be empty, it must contain valid chars.
        /// @exception std::invalid_argument if invalid
        ipv6_address(const std::string& v);
        /// Test if null (all 0's).
        bool is_null() const;
        /// Calculate string representation.
        /// Leading zeros are dropped, i.e. "3F" instead of "003F".
        /// Compress the longest run of 0's into "::" if
        /// \p compress is true.
        std::string string(bool compress = false) const;
        /// Stream out.
        /// @see string()
        std::ostream& write(std::ostream& os, bool compress = false) const;
        static const char SEPARATOR_CHAR; ///< separator (':')
    private:
        friend bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, ipv6_address& v);
        bool zero_run(size_t& first, size_t& last) const;
        unsigned short hextets_[8];
    };
    /// Stream out IP v6 address.
    inline std::ostream& operator <<(std::ostream& os, const ipv6_address& v) {return v.write(os);}
    /// Parse IP v6 address, returning whether found or not
    /// and advancing first and setting address if found.
    /// Does not skip leading space.
    bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, ipv6_address& v);
}
#endif
