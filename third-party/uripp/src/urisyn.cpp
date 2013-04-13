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
#include "urisyn.h"
namespace uripp {
    namespace urisyn {
        const char ENCODE_BEGIN_CHAR = '%';
        const traits SCHEME_TRAITS = {
            0, 0, ':',
            {
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CVA2,CINV,CVA2,CVA2,CINV,
                CVA2,CVA2,CVA2,CVA2,CVA2,CVA2,CVA2,CVA2, CVA2,CVA2,CEND,CINV,CINV,CINV,CINV,CINV,
                CINV,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CINV,CINV,CINV,CINV,CINV,
                CINV,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CINV,CINV,CINV,CINV,CINV, // 127 7F
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
            }
        };
        const traits AUTHORITY_TRAITS = {
            "//", 0, 0,
            {
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CEND,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, // 127 7F
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
            }
        };
        const traits PATH_TRAITS = {
            0, 0, 0,
            {   // '/' is invalid
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CVAL,CINV,CINV,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CINV,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CINV,CVAL,CINV,CINV,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CINV,CINV,CINV,CINV,CVAL,
                CINV,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CINV,CINV,CINV,CVAL,CINV, // 127 7F
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
            }
        };
        const traits QUERY_TRAITS = {
            0, '?', 0,
            {   // '=' and '&' are invalid
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CVAL,CINV,CINV,CVAL,CVAL,CINV,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CINV,CINV,CINV,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CINV,CINV,CINV,CINV,CVAL,
                CINV,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CINV,CINV,CINV,CVAL,CINV, // 127 7F
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
            }
        };
        const traits FRAGMENT_TRAITS = {
            0, '#', 0,
            {
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CVAL,CINV,CINV,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CINV,CVAL,CINV,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CINV,CINV,CINV,CINV,CVAL,
                CINV,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,
                CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL,CVAL, CVAL,CVAL,CVAL,CINV,CINV,CINV,CVAL,CINV, // 127 7F
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
                CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV, CINV,CINV,CINV,CINV,CINV,CINV,CINV,CINV,
            }
        };
        bool parse(const traits& ts, std::string::const_iterator& first, std::string::const_iterator last, std::string& comp, char* endc) {
            std::string::const_iterator f = first;
            char ec = 0;
            for (; f != last; ++f) {
                char c = *f;
                char cls = ts.char_class[(unsigned char)c];
                if (cls == CEND) { // End char.
                    ec = c;
                    break;
                }
                if (cls != CVAL && (f == first || cls != CVA2))
                    break;
            }
            if (f == first && !ec)
                return false;
            comp.assign(first, f);
            if (endc)
                *endc = ec;
            first = ec ? f + 1 : f;
            return true;
        }
        std::string encode(const traits& ts, const std::string& comp) {
            std::string::const_iterator f = comp.begin();
            std::string::const_iterator anchor = f;
            std::string s;
            for (; f != comp.end();) {
                char c = *f;
                if (ts.char_class[(unsigned char)c] < CVAL || c == ENCODE_BEGIN_CHAR) { // Must encode.
                    s.append(anchor, f); // Catch up to this char.
                    s.append(1, ENCODE_BEGIN_CHAR);
                    append_hex(c, s); // Convert.
                    anchor = ++f;
                }
                else
                    ++f;
            }
            return (anchor == comp.begin()) ? comp : s.append(f, comp.end());
        }
        bool decode(std::string& s) {
            size_t pos = s.find(ENCODE_BEGIN_CHAR);
            if (pos == std::string::npos) // Handle the "99%" case fast.
                return true;
            std::string v;
            for (size_t i = 0;;) {
                if (pos == std::string::npos) {
                    v.append(s, i, s.size() - i); // Append up to end.
                    break;
                }
                v.append(s, i, pos - i); // Append up to char.
                i = pos + 3; // Skip all 3 chars.
                char c;
                if (!parse_hex(s, pos + 1, c)) // Convert hex.
                    return false;
                v.insert(v.end(), c); // Append converted hex.
                pos = s.find(ENCODE_BEGIN_CHAR, i); // Find next
            }
            s = v;
            return true;
        }
    }
}
