// Copyright symbx 2007.
#include "utils.h"
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sstream>
#include <stdexcept>
#include <boost/algorithm/string.hpp>
#ifdef _WIN32
#define strtoll _strtoi64
#define strtoull _strtoui64
#endif
template<typename T> std::string stringstream_convert(const T& v) {
    std::ostringstream oss;
    oss << v;
    return oss.str();
}
static const char iso_8859_1_nonctl_nonpercnt_chars_[] = { // graphic + space + soft-hyphen - percent
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,0,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, // 3f
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,0, // 7f
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
};
namespace uripp {
    std::string convert(bool v) {return v ? "1" : "0";}
    std::string convert(int v) {return stringstream_convert(v);}
    std::string convert(unsigned int v) {return stringstream_convert(v);}
    std::string convert(unsigned long int v) {return stringstream_convert(v);}
    std::string convert(long long int v) {return stringstream_convert(v);}
    std::string convert(unsigned long long int v) {return stringstream_convert(v);}
    std::string convert(double v) {return stringstream_convert(v);}
    std::string convert(const std::string& v) {return boost::algorithm::trim_copy(v);}
    bool convert(const std::string& s, bool& v) {
        // Try common cases first.
        if (s == "1") {v = true; return true;}
        if (s == "0") {v = false; return true;}
        std::string b = boost::algorithm::trim_copy(s);
        if (b.empty())
            return false;
        boost::algorithm::to_lower(b);
        if (b == "1" || b == "on" || b == "t" || b == "true" || b == "y" || b == "yes") {
            v = true;
            return true;
        }
        if (b == "0" || b == "off" || b == "f" || b == "false" || b == "n" || b == "no") {
            v = false;
            return true;
        }
        throw std::invalid_argument("cannot convert to bool: " + s);
    }
    bool convert(const std::string& s, int& v) {
        if (s.empty())
            return false;
        char* endptr;
        errno = 0;
        long int t = strtol(s.c_str(), &endptr, 10);
        if (errno == ERANGE || (sizeof(int) < sizeof(long int) && (2147483647 < t || t < -2147483648))) {
            errno = 0;
            throw std::invalid_argument("cannot convert to int, out of range: " + s);
        }
        if (*endptr && !isspaces(endptr))
            throw std::invalid_argument("cannot convert to int, invalid chars: " + s);
        if (s.c_str() == endptr)
            return false;
        v = t;
        return true;
    }
    bool convert(const std::string& s, unsigned int& v) {
        if (s.empty())
            return false;
        char* endptr;
        errno = 0;
        unsigned long int t = strtoul(s.c_str(), &endptr, 10);
        if (errno == ERANGE || (sizeof(int) < sizeof(long int) && 4294967295 < t)) {
            errno = 0;
            throw std::invalid_argument("cannot convert to unsigned int, out of range: " + s);
        }
        if (*endptr && !isspaces(endptr))
            throw std::invalid_argument("cannot convert to unsigned int, invalid chars: " + s);
        if (s.c_str() == endptr)
            return false;
        v = t;
        return true;
    }
    bool convert(const std::string& s, unsigned long int& v) {
		if (sizeof(unsigned long int) == sizeof(unsigned int)) return convert(s, (unsigned int&)v);
		unsigned long long int i;
		if (convert(s, i)) {
			v = i;
			return true;
		}
		return false;
    }
    bool convert(const std::string& s, long long int& v) {
        if (s.empty())
            return false;
        char* endptr;
        errno = 0;
        long long int t = strtoll(s.c_str(), &endptr, 10);
        if (errno == ERANGE) {
            errno = 0;
            throw std::invalid_argument("cannot convert to long long int, out of range: " + s);
        }
        if (*endptr && !isspaces(endptr))
            throw std::invalid_argument("cannot convert to long long int, invalid chars: " + s);
        if (s.c_str() == endptr)
            return false;
        v = t;
        return true;
    }
    bool convert(const std::string& s, unsigned long long int& v) {
        if (s.empty())
            return false;
        char* endptr;
        errno = 0;
        unsigned long long int t = strtoull(s.c_str(), &endptr, 10);
        if (errno == ERANGE) {
            errno = 0;
            throw std::invalid_argument("cannot convert to unsigned long long int, out of range: " + s);
        }
        if (*endptr && !isspaces(endptr))
            throw std::invalid_argument("cannot convert to unsigned long long int, invalid chars: " + s);
        if (s.c_str() == endptr)
            return false;
        v = t;
        return true;
    }
//#	ifndef _WIN32
//    std::string convert(ptrdiff_t v) {
//		if (sizeof(ptrdiff_t) == sizeof(unsigned int)) return convert((unsigned int)v);
//		return convert((unsigned long long int)v);
//    }
//#	endif
    bool convert(const std::string& s, double& v) {
        if (s.empty())
            return false;
        char* endptr;
        errno = 0;
        double t = strtod(s.c_str(), &endptr);
        if (errno == ERANGE) {
            errno = 0;
            throw std::invalid_argument("cannot convert to double, out of range: " + s);
        }
        if (*endptr && !isspaces(endptr))
            throw std::invalid_argument("cannot convert to double, invalid chars: " + s);
        if (s.c_str() == endptr)
            return false;
        v = t;
        return true;
    }
    bool convert(const std::string& s, std::string& v) {
        if (s.empty() || isspaces(s.c_str()))
            return false;
        v = boost::algorithm::trim_copy(s);
        return true;
    }
    bool isspaces(const char* s) {
        for (; *s; ++s)
            if (!::isspace(*s))
                return false;
        return true;
    }
    bool parse_hex(const std::string& s, size_t pos, char& chr) {
        if (s.size() < pos + 2)
            return false;
        unsigned int v;
        unsigned int c = (unsigned int)s[pos];
        if ('0' <= c && c <= '9')
            v = (c - '0') << 4;
        else if ('A' <= c && c <= 'F')
            v = (10 + (c - 'A')) << 4;
        else if ('a' <= c && c <= 'f')
            v = (10 + (c - 'a')) << 4;
        else
            return false;
        c = (unsigned int)s[pos + 1];
        if ('0' <= c && c <= '9')
            v += c - '0';
        else if ('A' <= c && c <= 'F')
            v += 10 + (c - 'A');
        else if ('a' <= c && c <= 'f')
            v += 10 + (c - 'a');
        else
            return false;
        chr = (char)v; // Set output.
        return true;
    }
    void append_hex(char v, std::string& s) {
        unsigned int c = (unsigned char)v & 0xF0;
        c >>= 4;
        s.insert(s.end(), (char)((9 < c) ? (c - 10) + 'A' : c + '0'));
        c = v & 0x0F;
        s.insert(s.end(), (char)((9 < c) ? (c - 10) + 'A' : c + '0'));
    }
}
