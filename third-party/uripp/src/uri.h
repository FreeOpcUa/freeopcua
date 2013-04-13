#ifndef uripp_uri_h
#define uripp_uri_h
#include "apidefs.h"
#include "scheme.h"
#include "authority.h"
#include "path.h"
#include "query.h"
#include "fragment.h"
#include "utils.h"
#include <string>
#include <iostream>
/** \mainpage uripp - URI C++ Library
 *
 * \section feat_sec Features
 * <ol>
 * <li><a href="http://tools.ietf.org/html/rfc3986">RFC-3986</a> compliant (most features supported)</li>
 * <li>encodes and decodes URI</li>
 * <li>separate classes for each part of the URI, e.g. URI authority</li>
 * <li>path class supports easy composition and parsing of segments</li>
 * <li>query class has \p find method for finding and converting query parameters</li>
 * <li>conditional compilation for Microsoft DLLs (see apidefs.h)
 * </ol>
 *
 * \section req_sec Requirements
 * <ol>
 * <li><a href="http://en.wikipedia.org/wiki/C%2B%2B_standard_library">standard library</a></li>
 * <li><a href="http://www.boost.org/">boost library</a></li>
 * </ol>
 *
 * \section lic_sec License (MIT)
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

/// URI library.
namespace uripp {
    /** \brief Uniform Resource Identifier (URI) reference.
     *
     * A "URI-reference" is either a URI or a relative reference.
     *
     * Syntax: Note that it is up to the application to correctly
     * compose a URI reference with a missing scheme or authority, i.e.
     * to make sure the initial path chars (if any) do not look like
     * a scheme or authority (it helps with this library that
     * the \c path class cannot have empty segments so it cannot
     * have a URI authority-like prefix).
     *
     * <pre>
     * URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
     * relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
     * relative-part = "//" authority path-abempty
     *               | path-absolute
     *               | path-noscheme
     *               | path-empty
     * hier-part     = "//" authority path-abempty
     *               | path-absolute
     *               | path-rootless
     *               | path-empty
     * path-abempty  = *( "/" segment )
     * path-absolute = "/" [ segment-nz *( "/" segment ) ]
     * path-noscheme = segment-nz-nc *( "/" segment )
     * path-rootless = segment-nz *( "/" segment )
     * path-empty    = 0\<pchar>
     * </pre>
     *
     * Example:
     * \code
     * uripp::uri u("http://mydomain.com/a/b/c?id=12345&s=x%20y");
     * assert(u.scheme().string() == "http");
     * assert(u.authority().host() == "mydomain.com");
     * assert(u.path().absolute());
     * uripp::path::const_iterator it = u.path().begin();
     * assert(*it == "a" && *++it == "b" && *++it == "c" && ++it == u.path().end());
     * int id;
     * std::string s;
     * bool is_null;
     * assert(u.query().find("id", id, is_null) && id == 12345 && !is_null);
     * assert(u.query().find("s", s, is_null) && s == "x y" && !is_null);
     * assert(!u.query().find("foo", s, is_null));
     * \endcode
     * @see http://tools.ietf.org/html/rfc3986 */
    class URIPP_API uri {
    public:
        typedef uripp::scheme scheme_type; ///< scheme type
        typedef uripp::authority authority_type; ///< authority type
        typedef uripp::path path_type; ///< path type
        typedef uripp::query query_type; ///< query type
        typedef uripp::fragment fragment_type; ///< fragment type
        uri(); ///< Construct.
        /// Construct from encoded string.
        /// @exception std::invalid_argument if invalid encoding
        uri(const std::string& v);
        bool empty() const {return is_null();} ///< Test if null/empty.
        bool is_null() const; ///< Test if null/empty.
        bool relative() const {return scheme_.is_null();} ///< Test if relative (null scheme).
        const scheme_type& scheme() const {return scheme_;} ///< Get scheme (null if relative).
        scheme_type& scheme() {return scheme_;} ///< Get scheme (null if relative).
        const authority_type& authority() const {return authority_;} ///< Get authority.
        authority_type& authority() {return authority_;} ///< Get authority.
        const path_type& path() const {return path_;} ///< Get path.
        path_type& path() {return path_;} ///< Get path.
        const query_type& query() const {return query_;} ///< Get query.
        query_type& query() {return query_;} ///< Get query.
        const fragment_type& fragment() const {return fragment_;} ///< Get fragment.
        fragment_type& fragment() {return fragment_;} ///< Get fragment.
        std::string encoding() const; ///< Calculate encoded string.
        std::ostream& operator <<(std::ostream& os) const; ///< Stream out encoding.
    private:
        friend bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, uri& v, std::string* errs);
        static bool parse_literal(std::string::const_iterator& first, std::string::const_iterator last, const char* v);
        scheme_type scheme_;
        authority_type authority_;
        path_type path_;
        query_type query_;
        fragment_type fragment_;
    };
    /// Parse URI, returning whether found or not and advancing first
    /// and setting URI if found. Does not skip leading space.
    ///
    /// If \p errs is specified parsing is more lax allowing decoding
    /// and other errors and setting \p errs with the error messages.
    /// See the individual component \c parse functions for details.
    bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, uri& v, std::string* errs = 0);
    /// Stream out URI encoding.
    inline std::ostream& operator <<(std::ostream& os, const uri& v) {return v.operator <<(os);}
}
#endif
