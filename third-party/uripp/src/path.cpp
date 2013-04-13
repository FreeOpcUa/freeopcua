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
#include "path.h"
#include "urisyn.h"
#include <stdexcept>
namespace uripp {
    const char path::SEPARATOR_CHAR = '/';
    path::path() : absolute_(false), is_directory_(false) {}
    path::path(const std::string& v) : absolute_(false), is_directory_(false) {
        if (!v.empty()) {
            std::string::const_iterator first = v.begin();
            if (!parse(first, v.end(), *this) || first != v.end())
                throw std::invalid_argument("invalid URI path: \"" + v + "\"");
        }
    }
    void path::clear() {
        absolute_ = false;
        is_directory_ = false;
        segments_.clear();
    }
    bool path::match_prefix(const path& rhs) const {
        if (absolute_ != rhs.absolute_)
            return false;
        const_iterator itrhs = rhs.begin();
        for (const_iterator it = begin();; ++it, ++itrhs) {
            if (it == end()) {
                if (itrhs != rhs.end())
                    return false;
                break;
            }
            if (itrhs == rhs.end())
                return true;
            if (*it != *itrhs)
                return false;
        }
        return !rhs.is_directory_ || is_directory_;
    }
    const std::string& path::front() const {
        if (segments_.empty())
            throw std::out_of_range("attempt to access empty URI path");
        return segments_.front();
    }
    const std::string& path::back() const {
        if (segments_.empty())
            throw std::out_of_range("attempt to access empty URI path");
        return segments_.back();
    }
    void path::pop_front() {
        if (!segments_.empty()) {
            segments_.pop_front();
            absolute_ = false;
            if (segments_.empty())
                is_directory_ = false;
        }
    }
    bool path::pop_back(const path& back) {
        if (is_directory_ != back.is_directory_ || back.empty())
            return false;
        segments_type::reverse_iterator rit = segments_.rbegin();
        segments_type::reverse_iterator rend = segments_.rend();
        segments_type::const_reverse_iterator ritb = back.segments_.rbegin();
        segments_type::const_reverse_iterator rendb = back.segments_.rend();
        bool found = false;
        for (; rit != rend && *rit == *ritb; ++rit)
            if (++ritb == rendb) { // Matched all.
                found = true;
                break;
            }
        if (!found)
            return false;
        // Go on to the front of the list, and in from the front.
        segments_type::iterator it = segments_.begin();
        for (; ++rit != rend;)
            ++it;
        if (back.absolute_ && (it != segments_.begin() || !absolute_))
            return false;
        // Erase to end.
        segments_.erase(it, segments_.end());
        // Adjust directory bit if we are empty.
        if (segments_.empty())
            is_directory_ = false;
        return true;
    }
    bool path::operator ==(const path& rhs) const {
        return absolute_ == rhs.absolute_ && is_directory_ == rhs.is_directory_ && segments_ == rhs.segments_;
    }
    bool path::operator <(const path& rhs) const {
        if (!absolute_ && rhs.absolute_) return true;
        if (absolute_ && !rhs.absolute_) return false;
        segments_type::const_iterator it = segments_.begin();
        segments_type::const_iterator itr = rhs.segments_.begin();
        for (;; ++it, ++itr) {
            if (it == segments_.end() && itr != rhs.segments_.end()) return true;
            if (it != segments_.end() && itr == rhs.segments_.end()) return false;
            int i = it->compare(*itr);
            if (i < 0) return true;
            if (0 < i) return false;
        }
        return !is_directory_ && rhs.is_directory_;
    }
    path& path::operator +=(const std::string& rhs) {
        segments_.push_back(rhs);
        is_directory_ = false;
        return *this;
    }
    path& path::operator +=(const path& rhs) {
        segments_.insert(segments_.end(), rhs.segments_.begin(), rhs.segments_.end());
        is_directory_ = rhs.is_directory_;
        return *this;
    }
    std::string path::encoding() const {
        std::string s;
        if (absolute_)
            s += SEPARATOR_CHAR;
        for (segments_type::const_iterator it = segments_.begin(); it != segments_.end(); ++it) {
            if (it != segments_.begin())
                s += SEPARATOR_CHAR;
            s += urisyn::encode(urisyn::PATH_TRAITS, *it); // Encode and append.
        }
        if (is_directory_)
            s += SEPARATOR_CHAR;
        return s;
    }
    bool parse(std::string::const_iterator& first, std::string::const_iterator last, path& v, std::string* errs) {
        std::string::const_iterator f = first;
        path tmp;
        std::string e;
        if (f != last && *f == path::SEPARATOR_CHAR) { // Leading separator: absolute.
            tmp.absolute_ = true;
            ++f;
        }
        bool sep = true; // Ends in separator.
        for (; f != last;)
            if (*f == path::SEPARATOR_CHAR) // Empty segment: ignore.
                ++f;
            else { // Consume segment.
                std::string s;
                urisyn::parse(urisyn::PATH_TRAITS, f, last, s);
                if (!s.empty()) { // Non-empty segment: decode and push.
                    if (!urisyn::decode(s)) { // Invalid encoding in segment.
                        if (!errs)
                            return false;
                        e += ", (" + convert((size_t)(f - first)) + ") invalid segment encoding (using as is)";
                    }
                    tmp.segments_.push_back(s);
                }
                if (f == last || *f != path::SEPARATOR_CHAR) { // Ends in some invalid char: break.
                    sep = false;
                    break;
                }
                ++f;
            }
        if (f == first)
            return false;
        if (errs && !e.empty()) {
            if (!errs->empty())
                *errs += "; ";
            *errs += "URI path: \"" + std::string(first, f) + "\"" + e;
        }
        if (sep && !tmp.segments_.empty())
            tmp.is_directory_ = true;
        v = tmp;
        first = f;
        return true;
    }
    std::string convert(const path& v) {return v.encoding();}
    bool convert(const std::string& s, path& v) {
        if (s.empty() || isspaces(s.c_str()))
            return false;
        v = s;
        return true;
    }
}
