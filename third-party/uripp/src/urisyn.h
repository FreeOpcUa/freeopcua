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
#ifndef uripp_urisyn_h
#define uripp_urisyn_h
#include "apidefs.h"
#include "utils.h"
#include <string>
namespace uripp {
    /// URI syntax declarations.
    namespace urisyn {
        /// Char class.
        enum char_class_e {
            CINV = -2, ///< invalid
            CEND = -1, ///< end delimitor
            CVAL = 0, ///< valid any position
            CVA2 = 1, ///< valid anywhere but 1st position
        };
        /// Traits used for parsing and encoding components.
        struct URIPP_API traits {
            const char* begin_cstring; ///< begin cstring (or 0 if none)
            const char begin_char; ///< begin char (or 0 if none)
            const char end_char; ///< end char (or 0 if none)
            char char_class[256]; ///< map of char to class
        };
        /// Parse the URI componet, returning whether successful and setting
        /// the string and end char and advancing if so.
        /// This looks for end chars either returning the string up to, but not
        /// including, the end char or returning the string up to the first
        /// character that is not valid for the component.
        /// The arg \p first is advanced after the end char if there is one.
        /// The arg \p endc is set to either 0 if no end char found or to
        /// the end char. If the first char is an end char then
        /// true is returned and the string will be empty.
        /// Leading white space is not skipped.
        ///
        /// The component is not decoded and should not be decoded until
        /// after futher parsing with the component's subdelimiters, if any.
        bool URIPP_API parse(const traits& ts, std::string::const_iterator& first, std::string::const_iterator last, std::string& comp, char* endc = 0);
        /// Encode the URI (sub) component. Note that this should be used on the
        /// subcomponents before appending to subdelimiter chars, if any.
        ///
        /// From the RFC: URI producing applications should percent-encode data octets
        /// that correspond to characters in the reserved set unless these characters
        /// are specifically allowed by the URI scheme to represent data in that
        /// component.  If a reserved character is found in a URI component and
        /// no delimiting role is known for that character, then it must be
        /// interpreted as representing the data octet corresponding to that
        /// character's encoding in US-ASCII.
        /// @see http://tools.ietf.org/html/rfc3986
        /// @see decode
        std::string URIPP_API encode(const traits& ts, const std::string& comp);
        /// Decode the pct-encoded (hex) sequences, if any, return success.
        /// Does not change string on error.
        /// @see http://tools.ietf.org/html/rfc3986#section-2.1
        /// @see encode
        bool URIPP_API decode(std::string& s);
        extern const char URIPP_API ENCODE_BEGIN_CHAR; ///< encode begin char ('\%')
        extern const traits URIPP_API SCHEME_TRAITS; ///< scheme traits
        extern const traits URIPP_API AUTHORITY_TRAITS; ///< authority traits
        extern const traits URIPP_API PATH_TRAITS; ///< path traits
        extern const traits URIPP_API QUERY_TRAITS; ///< query traits
        extern const traits URIPP_API FRAGMENT_TRAITS; ///< fragment traits
    }
}
#endif
