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
#ifndef uripp_query_h
#define uripp_query_h
#include "apidefs.h"
#include "utils.h"
#include <string>
#include <vector>
#include <iostream>
namespace uripp {
    /** \brief URI query component.
     *
     * This parses the query component into a list of
     * string pairs, the first is the key and the second the value.
     *
     * Syntax (see uri_path for additional definitions): Both key and
     * value are pct-decoded after parsing.
     * <pre>
     * query   = kv-pair *( "&" kv-pair )
     * kv-pair = key "=" [ value ]
     * key     = 1*qchar
     * value   = 1*qchar
     * qchar   = \<pchar except "=" and "&"> | "/" | "?"
     * </pre>
     *
     * Note that this provides optional sorting for faster finds
     * when there are a non-trivial number of pairs.
     * However, once the query is sorted pairs must not be inserted.
     * @see http://tools.ietf.org/html/rfc3986#section-3.4 */
    class URIPP_API query : public std::vector<std::pair<std::string, std::string> > {
    public:
        query(); ///< Construct.
        /// Construct from encoded string, for example "a=foo%20bar&b=1",
        /// and sort the keys if \p dosort is true.
        /// Note that this should not contain the leading '?'.
        /// See \c parse() for less strict construction.
        /// @exception std::invalid_argument if missing key or invalid encoding
        query(const std::string& v, bool dosort = false);
        /// Sort the pairs by key. This will speed the find methods if there
        /// are more than a trivial number of pairs.
        void sort();
        bool sorted() const {return sorted_;} ///< Test if sort() has been called.
        std::string encoding() const; ///< Calculate encoded string.
        /// Find the const_iterator for the key. Note the search is linear
        /// if not sorted.
        const_iterator find(const std::string& key) const;
        /// Find the iterator for the key. Note the search is linear
        /// if not sorted.
        iterator find(const std::string& key);
        /// Find the key and convert its associated value, returning true
        /// if the key is found and is_null true if the key is found but the
        /// value string is empty or isspace. The value is only set if found
        /// but not null. Note the search is linear.
        /// @exception std::invalid_argument if not null and cannot convert
        template<typename T> bool find(const std::string& key, T& value, bool& is_null) const {
            const_iterator it = find(key);
            if (it == end())
                return false;
            is_null = !convert(it->second, value);
            return true;
        }
        static const char PAIRS_SEP_CHAR; ///< pairs separator char ('&')
        static const char KEY_VALUE_SEP_CHAR; ///< key-value separator char ('=')
    private:
        friend bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, query& v, std::string* errs);
        bool sorted_;
    };
    /// Parse URI query, returning whether found or not
    /// and advancing first and setting query if found.
    /// Does not skip leading space.
    ///
    /// If \p errs is specified the following take place:<ul>
    /// <li>Errors in parsing subcomponents, after the first, do not
    ///     cause an immediate false return and errors messages
    ///     are reported in the error string \p errs.</li>
    /// <li>Errors are handled as follows:<ul>
    ///     <li>Key-value pairs with an empty key are ignored.</li>
    ///     <li>Keys with missing '=' are ignored.</li>
    ///     <li>Errors decoding a key or value result in no
    ///         decoding at all (i.e. assumes an unencoded '%').</li>
    ///     </ul></li>
    /// </ul>
    bool URIPP_API parse(std::string::const_iterator& first, std::string::const_iterator last, query& v, std::string* errs = 0);
    /// Stream out query encoding.
    inline std::ostream& operator <<(std::ostream& os, const query& v) {return os << v.encoding();}
}
#endif
