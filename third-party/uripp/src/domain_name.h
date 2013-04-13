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
#ifndef uripp_domain_name_h
#define uripp_domain_name_h
#include "apidefs.h"
#include <string>
#include <iostream>
namespace uripp {
    /** \brief Domain name.
     *
     * Syntax: "Labels" separated by '.'. Labels must start and
     * end with a (ASCII) letter or digit, and have as interior
     * characters only letters, digits, and hyphen. Not case sensitive,
     * but canonical form is lower case.
     * @see http://tools.ietf.org/html/rfc1034#section-3.5 */
    class URIPP_API domain_name {
    public:
        domain_name(); ///< Construct null.
        /// Construct from string. This will convert to lower case
        /// and check for valid chars. Note that the string cannot
        /// be empty, it must contain valid chars.
        /// @exception std::invalid_argument if invalid
        domain_name(const std::string& v);
        bool is_null() const {return string_.empty();} ///< Test if null.
        const std::string& string() const {return string_;} ///< Get string.
        /// Append domain name and return result.
        domain_name operator +(const domain_name& rhs) const {
            domain_name result = *this;
            return result += rhs;
        }
        /// Append domain name to this and return this.
        domain_name& operator +=(const domain_name& rhs);
        static const char SEPARATOR_CHAR; ///< separator ('.')
        static const char char_class[256]; ///< char class
    private:
        friend bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, domain_name& v);
        std::string string_;
    };
    /// Stream out domain name.
    inline std::ostream& operator <<(std::ostream& os, const domain_name& v) {return os << v.string();}
    /// Parse domain name, returning whether found or not
    /// and advancing first and setting name if found.
    /// Does not skip leading space.
    bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, domain_name& v);
}
#endif
