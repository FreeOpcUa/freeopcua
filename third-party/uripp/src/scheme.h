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
#ifndef uripp_scheme_h
#define uripp_scheme_h
#include "apidefs.h"
#include <string>
#include <iostream>
namespace uripp {
    /** \brief URI scheme component.
     *
     * Syntax:
     * <pre>
     * scheme = ALPHA *( ALPHA | DIGIT | "+" | "-" | "." )
     * </pre>
     * @see http://tools.ietf.org/html/rfc3986#section-3.1 */
    class URIPP_API scheme {
    public:
        scheme(); ///< Construct null.
        /// Construct from string. This will convert to lower case
        /// and check for valid chars. Does not include the ":".
        /// Note that the string cannot be empty, it must contain
        /// chars that are valid for the scheme.
        /// @exception std::invalid_argument if invalid
        scheme(const std::string& v);
        bool empty() const {return is_null();} ///< Test if null/empty.
        bool is_null() const {return string_.empty();} ///< Test if null/empty.
        const std::string& string() const {return string_;} ///< Get string.
    private:
        friend bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, scheme& v, char* endc);
        std::string string_;
    };
    /// Stream out URI scheme.
    inline std::ostream& operator <<(std::ostream& os, const scheme& v) {return os << v.string();}
    /// Parse URI scheme, returning whether found or not
    /// and advancing first and setting scheme and end char if found.
    /// Does not skip leading space.
    bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, scheme& v, char* endc = 0);
}
#endif
