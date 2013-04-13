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
#ifndef uripp_fragment_h
#define uripp_fragment_h
#include "apidefs.h"
#include <string>
#include <iostream>
namespace uripp {
    /** \brief URI fragment component.
     *
     * Syntax (see uri_path for additional definitions): String
     * is pct-decoded after parsing.
     * <pre>
     * fragment = *( pchar | "/" | "?" )
     * </pre>
     * @see http://tools.ietf.org/html/rfc3986#section-3.5 */
    class URIPP_API fragment {
    public:
        fragment(); ///< Construct.
        /// Construct from encoded string.
        /// Note that this should not contain the leading '#'.
        /// See \c parse() for less strict construction.
        /// @exception std::invalid_argument if invalid encoding
        fragment(const std::string& v);
        bool empty() const {return is_null();} ///< Test if null/empty.
        bool is_null() const {return string_.empty();} ///< Test if null/empty.
        const std::string& string() const {return string_;} ///< Get decoded string.
        std::string encoding() const; ///< Calculate encoded string.
    private:
        friend bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, fragment& v, std::string* errs);
        std::string string_;
    };
    /// Parse URI fragment, returning whether found or not
    /// and advancing first and setting fragment if found.
    /// Does not skip leading space.
    ///
    /// If \p errs is specified the following take place:<ul>
    /// <li>Errors in decoding the string do not
    ///     cause an immediate false return and and error message
    ///     is reported in the error string \p errs. The string
    ///     is used without decoding (i.e. assumes an unencoded '%').</li>
    /// </ul>
    bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, fragment& v, std::string* errs = 0);
    /// Stream out fragment encoding.
    inline std::ostream& operator <<(std::ostream& os, const fragment& v) {return os << v.encoding();}
}
#endif
