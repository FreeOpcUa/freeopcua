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
#ifndef uripp_path_h
#define uripp_path_h
#include "apidefs.h"
#include <string>
#include <list>
#include <iostream>
namespace uripp {
    /** \brief URI path component.
     *
     * Note that there are potential ambiguities
     * when using ":" and "//" in paths in URIs (see RFC).
     *
     * Syntax: Segments are pct-decoded after parsing.
     * <pre>
     * ALPHA      =  %x41-5A | %x61-7A   ; A-Z | a-z
     * DIGIT      =  %x30-39             ; 0-9
     * sub-delims = "!" | "$" | "&" | "'" | "(" | ")" | "*"
     *            | "+" | "," | ";" | "="
     * unreserved = ALPHA | DIGIT | "-" | "." | "_" | "~"
     * pchar      = unreserved | pct-encoded | sub-delims | ":" | "@"
     * segment    = *pchar
     * path       = *( "/" segment )
     * </pre>
     * @see http://tools.ietf.org/html/rfc3986#section-3.3 */
    class URIPP_API path {
    public:
        typedef std::list<std::string> segments_type; ///< segments type
        typedef segments_type::const_iterator const_iterator; ///< segments const iterator type
        path(); ///< Construct.
        /// Construct from encoded string. For example "/foo/bar" or
        /// "/foo/ba%20r". Empty segments are removed, i.e. "a//b"
        /// becomes "a/b". See \c parse() for less strict construction.
        /// @exception std::invalid_argument if invalid encoding
        path(const std::string& v);
        /// Test if empty and not absolute.
        bool empty() const {return segments_.empty() && !absolute_;}
        /// Test if absolute, meaning path begins with "/".
        bool absolute() const {return absolute_;}
        /// Set whether absolute or not.
        void absolute(bool v) {absolute_ = v;}
        /// Test if a directory, meaning the path ends in a "/".
        bool is_directory() const {return is_directory_;}
        /// Set whether a directory or not.
        void is_directory(bool v) {is_directory_ = v;}
        /// Get front segment.
        /// @exception std::out_of_range if empty
        const std::string& front() const;
        /// Get back segment.
        /// @exception std::out_of_range if empty
        const std::string& back() const;
        /// Test if the given other URI path is a prefix of this.
        /// Must match whether absolute as well.
        bool match_prefix(const path& rhs) const;
        void pop_front(); ///< Pop front segment and reset absolute.
        /// Remove the back of the path if it matches the
        /// argument (including whether directory or not) and
        /// return whether removed/matched. Note that if this
        /// starts as a directory it stays a directory. If back is
        /// absolute and it does not match all of this or this
        /// is not absolute then there is no match.
        bool pop_back(const path& back);
        void clear(); ///< Clear segments and reset absolute and is_directory.
        size_t size() const {return segments_.size();} /// Get number of segments.
        bool operator ==(const path& rhs) const; ///< Equal operator.
        bool operator !=(const path& rhs) const {return !(*this == rhs);} ///< Not equal operator.
        /// Less operator. Examines the following in order:
        /// <ol><li>relative are less than absolute</li>
        /// <li>lexicographical comparison of segments</li>
        /// <li>non-directories are less than directories</li></ol>
        bool operator <(const path& rhs) const;
        /// Append unencoded segment and reset is_directory.
        path operator +(const std::string& rhs) const {
            path result = *this;
            return result += rhs;
        }
        path& operator +=(const std::string& rhs); ///< Append unencoded segment and reset is_directory.
        /// Append path and set is_directory according to rhs.
        path operator +(const path& rhs) const {
            path result = *this;
            return result += rhs;
        }
        path& operator +=(const path& rhs); ///< Append path and set is_directory according to rhs.
        std::string encoding() const; ///< Calculate encoded string.
        const_iterator begin() const {return segments_.begin();} ///< Get iterator at beginning.
        const_iterator end() const {return segments_.end();} ///< Get iterator at end.
        static const char SEPARATOR_CHAR; ///< separator ('/')
    private:
        friend bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, path& v, std::string* errs);
        bool absolute_;
        bool is_directory_;
        segments_type segments_;
    };
    /// Parse URI path, returning whether found or not
    /// and advancing first and setting path if found.
    /// Does not skip leading space.
    ///
    /// If \p errs is specified the following take place:<ul>
    /// <li>Errors in decoding segments do not
    ///     cause an immediate false return and and error message
    ///     is reported in the error string \p errs. The segment
    ///     is used without decoding (i.e. assumes an unencoded '%').</li>
    /// </ul>
    bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, path& v, std::string* errs = 0);
    std::string URIPP_API convert(const path& v); ///< URI path to string
    /// String to URI path, returning true if set, which is
    /// when the string is not empty or all isspace.
    /// isspace before and/or after value OK.
    /// @exception std::invalid_argument on conversion error
    bool URIPP_API convert(const std::string& s, path& v);
    /// Stream out path encoding.
    inline std::ostream& operator <<(std::ostream& os, const path& v) {return os << v.encoding();}
}
#endif
