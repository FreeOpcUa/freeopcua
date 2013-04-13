#include "test.h"
#include "../src/urisyn.h"
#include "../src/uri.h"
#include <iostream>
#include <stdexcept>
#include <assert.h>
using namespace std;
using namespace uripp;
static void test_decode() {
    {string s = ""; TEST_ASSERT(urisyn::decode(s) && s  == "");}
    {string s = "a"; TEST_ASSERT(urisyn::decode(s) && s  == "a");}
    {string s = "a-+.."; TEST_ASSERT(urisyn::decode(s) && s  == "a-+..");}
    {string s = "//\n"; TEST_ASSERT(urisyn::decode(s) && s  == "//\n");}
    {string s = "%20"; TEST_ASSERT(urisyn::decode(s) && s  == " ");}
    {string s = "%20%5A"; TEST_ASSERT(urisyn::decode(s) && s  == " Z");}
    {string s = "%61%5a"; TEST_ASSERT(urisyn::decode(s) && s  == "aZ");}
    {string s = "%41%7a"; TEST_ASSERT(urisyn::decode(s) && s  == "Az");}
    {string s = "1%412%7a."; TEST_ASSERT(urisyn::decode(s) && s  == "1A2z.");}
    {string s = "1%412%"; TEST_ASSERT(!urisyn::decode(s) && s  == "1%412%");}
    {string s = "%"; TEST_ASSERT(!urisyn::decode(s) && s  == "%");}
    {string s = "1%412%7"; TEST_ASSERT(!urisyn::decode(s) && s  == "1%412%7");}
    {string s = "1%412%7g"; TEST_ASSERT(!urisyn::decode(s) && s  == "1%412%7g");}
    {string s = "1%412%q7"; TEST_ASSERT(!urisyn::decode(s) && s  == "1%412%q7");}
}
static void test_scheme() {
    {scheme v; TEST_ASSERT(v.is_null());}
    {scheme v("A1+.-aZz"); TEST_ASSERT(v.string() == "a1+.-azz");}
    try {scheme v("1abc"); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    try {scheme v("ab,c"); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    try {scheme v("ab "); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    {
        string s("abc:"); scheme v;
        std::string::const_iterator it = s.begin();
        char ec;
        TEST_ASSERT(parse(it, s.end(), v, &ec) && v.string() == "abc" && ec == ':');
    } {
        string s("A12z+.- "); scheme v;
        std::string::const_iterator it = s.begin();
        char ec;
        TEST_ASSERT(parse(it, s.end(), v, &ec) && v.string() == "a12z+.-" && *it == ' ' && !ec);
    }
}
static void test_authority() {
    {authority v; TEST_ASSERT(v.is_null());}
    {
        authority v("foo.com");
        TEST_ASSERT(!v.is_null() && v.host_type() == authority::DOMAIN_NAME && v.string() == "foo.com");
    } {
        authority v("foo.com:345");
        TEST_ASSERT(v.host_type() == authority::DOMAIN_NAME && v.port() == 345 && v.string() == "foo.com:345");
    } {
        authority v("1.2.3.4");
        TEST_ASSERT(v.host_type() == authority::IP_ADDRESS && v.string() == "1.2.3.4");
    } {
        authority v("1.2.3.4:65535");
        TEST_ASSERT(v.host_type() == authority::IP_ADDRESS && v.port() == 65535 && v.string() == "1.2.3.4:65535");
    } {
        authority v("[1::9]");
        TEST_ASSERT(v.host_type() == authority::IPV6_ADDRESS && v.string() == "[1::9]");
    } {
        authority v("[1::9]:876");
        TEST_ASSERT(v.host_type() == authority::IPV6_ADDRESS && v.port() == 876 && v.string() == "[1::9]:876");
    }
    try {authority v("abc.com "); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    {
        authority v; string s("[1::9]:876."); string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.host_type() == authority::IPV6_ADDRESS && v.port() == 876 && v.string() == "[1::9]:876" && *it == '.');
    }
}
static void test_path() {
    /// sub-delims = "!" | "$" | "&" | "'" | "(" | ")" | "*"
    ///            | "+" | "," | ";" | "="
    /// unreserved = ALPHA | DIGIT | "-" | "." | "_" | "~"
    /// pchar = unreserved | pct-encoded | sub-delims | ":" | "@"
    {
        path v;
        TEST_ASSERT(v.empty());
    } {
        path v("");
        TEST_ASSERT(v.empty() && !v.size() && v.encoding() == "" && !v.absolute() && !v.is_directory());
    } {
        path v("/");
        TEST_ASSERT(!v.empty() && !v.size() && v.encoding() == "/" && v.absolute() && !v.is_directory());
    } {
        path v("//");
        TEST_ASSERT(!v.empty() && !v.size() && v.encoding() == "/" && v.absolute() && !v.is_directory());
    } {
        string s = "a";
        path v(s);
        TEST_ASSERT(v.encoding() == s && !v.empty() && v.size() == 1 && !v.absolute() && !v.is_directory());
    } {
        string s = "/a";
        path v(s);
        TEST_ASSERT(v.encoding() == s && !v.empty() && v.size() == 1 && v.absolute() && !v.is_directory());
    } {
        string s = "/a/";
        path v(s);
        TEST_ASSERT(v.encoding() == s && !v.empty() && v.size() == 1 && v.absolute() && v.is_directory());
    } {
        string s = "a/";
        path v(s);
        TEST_ASSERT(v.encoding() == s && !v.empty() && v.size() == 1 && !v.absolute() && v.is_directory());
    } {
        string s = "/a/b";
        path v(s);
        TEST_ASSERT(v.front() == "a" && v.encoding() == s && !v.empty() && v.size() == 2 && v.absolute() && !v.is_directory());
    } {
        string s = "/a/b";
        path v("/a//b");
        TEST_ASSERT(v.front() == "a" && v.encoding() == s && !v.empty() && v.size() == 2 && v.absolute() && !v.is_directory());
    } {
        string s = "!$&'()*+,;=-._~:@";
        path v(s);
        TEST_ASSERT(v.encoding() == s);
    } {
        path v("%41");
        TEST_ASSERT(v.encoding() == "A");
    } {
        path v("%411%412%7a./%7a%2F");
        TEST_ASSERT(v.encoding() == "A1A2z./z%2F");
        path v1("/abc/");
        TEST_ASSERT((v + v1).encoding() == "A1A2z./z%2F/abc/");
        v += v1;
        TEST_ASSERT(v.encoding() == "A1A2z./z%2F/abc/");
        v += "b";
        TEST_ASSERT(v.encoding() == "A1A2z./z%2F/abc/b");
        TEST_ASSERT((v + "c").encoding() == "A1A2z./z%2F/abc/b/c");
        v.pop_front();
        TEST_ASSERT(v.front() == "z/" && v.encoding() == "z%2F/abc/b" && v.size() == 3);
        v.pop_front();
        TEST_ASSERT(v.front() == "abc" && v.encoding() == "abc/b");
        v.is_directory(true);
        TEST_ASSERT(v.front() == "abc" && v.encoding() == "abc/b/");
    } {
        string s = "/a%2F";
        path v;
        v += "a/";
        v.absolute(true);
        TEST_ASSERT(v.encoding() == s && !v.empty() && v.size() == 1 && v.absolute() && !v.is_directory());
    } {
        string s = "a%25/";
        path v;
        v += "a%";
        v.is_directory(true);
        TEST_ASSERT(v.encoding() == s && !v.empty() && v.size() == 1 && !v.absolute() && v.is_directory());
    } {
        const path v("a/b/c");
        string s;
        for (path::const_iterator it = v.begin(); it != v.end(); ++it)
            s += *it + ".";
        TEST_ASSERT(s == "a.b.c.");
    } {
        path v("a/b/c/");
        path v1("b/c/");
        TEST_ASSERT(v.pop_back(v1) && v.encoding() == "a/");
    } {
        path v("a/b/c");
        path v1("d/c");
        TEST_ASSERT(!v.pop_back(v1));
    } {
        path v("a/b/c");
        path v1("/a/b/c");
        TEST_ASSERT(!v.pop_back(v1));
    } {
        path v("/a/b/c");
        path v1("/a/b/c");
        TEST_ASSERT(v.pop_back(v1) && !v.size() && v.absolute());
    }
    try {path v("%"); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    try {path v("aaaaaaa%1"); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    {
        path v("/a/b/c");
        TEST_ASSERT(v.match_prefix(path("/")) && v.match_prefix(path("/a/")) && !v.match_prefix(path("a")));
        TEST_ASSERT(!v.match_prefix(path("/a/b/c/")) && !v.match_prefix(path("/a/b/c/d")) && v.match_prefix(path("/a/b")));
    }
    {   // bad encoding
        path v; string s("/a/b/c%"); string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v));
    } {
        path v; string s("/a/b/c "); string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.encoding() == "/a/b/c" && *it == ' ');
    } {
        path v; string s("/ "); string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.encoding() == "/" && *it == ' ');
    } {
        path v; string s("/?"); string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.encoding() == "/" && *it == '?');
    } {
        path v; string s("#"); string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v) && v.empty() && *it == '#');
    } {
        path v; string s("% "); string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v) && *it == '%');
    } {
        path v; string s("% "); string::const_iterator it = s.begin(); string errs;
        TEST_ASSERT(parse(it, s.end(), v, &errs) && v.encoding() == "%25" && *it == ' ' && !errs.empty());
    }
}
static void test_query() {
    {query v; TEST_ASSERT(v.empty());}
    {query v(""); TEST_ASSERT(v.empty());}
    {query v("&&&");TEST_ASSERT(v.empty());}
    {query v("&&=&"); TEST_ASSERT(v.empty() && v.encoding() == "");}
    {
        query v("a=1");
        query::const_iterator it = v.begin();
        TEST_ASSERT(!v.empty() && it->first == "a" && it->second == "1" && v.size() == 1);
        TEST_ASSERT(v.encoding() == "a=1");
    } {
        query v("a=1&");
        query::const_iterator it = v.begin();
        TEST_ASSERT(!v.empty() && it->first == "a" && it->second == "1" && v.size() == 1);
        TEST_ASSERT(v.encoding() == "a=1");
    } {
        query v("a=1&b=%20&");
        query::const_iterator it = v.begin();
        TEST_ASSERT(!v.empty() && it->first == "a" && it++->second == "1" && v.size() == 2);
        TEST_ASSERT(it->first == "b" && it->second == " ");
        TEST_ASSERT(v.encoding() == "a=1&b=%20");
    } {
        query v("z=1&b=%20&check=on&cat=%3d&t=");
        query::const_iterator it = v.begin();
        TEST_ASSERT(!v.empty() && it->first == "z" && it++->second == "1" && v.size() == 5);
        TEST_ASSERT(it->first == "b" && it++->second == " ");
        TEST_ASSERT(it->first == "check" && it++->second == "on");
        TEST_ASSERT(it->first == "cat" && it++->second == "=");
        TEST_ASSERT(it->first == "t" && it++->second == "");
        TEST_ASSERT(v.encoding() == "z=1&b=%20&check=on&cat=%3D&t=");
        int i;
        bool is_null = true;
        TEST_ASSERT(v.find("z", i, is_null) && i == 1 && !is_null);
        string s;
        is_null = true;
        TEST_ASSERT(v.find("cat", s, is_null) && s == "=" && !is_null);
        bool b;
        is_null = true;
        TEST_ASSERT(v.find("check", b, is_null) && b && !is_null);
        TEST_ASSERT(v.find("t", b, is_null) && is_null);
        TEST_ASSERT(!v.find("foo", b, is_null));
        v.sort();
        is_null = true; s.clear();
        TEST_ASSERT(v.find("cat", s, is_null) && s == "=" && !is_null);
        i = 0;
        TEST_ASSERT(v.find("z", i, is_null) && i == 1 && !is_null);
        is_null = false;
        TEST_ASSERT(v.find("b", s, is_null) && is_null);
        b = false;
        TEST_ASSERT(v.find("check", b, is_null) && b && !is_null);
        TEST_ASSERT(!v.find("foo", b, is_null));
    }
    {query v("a=1&b=pppppaaaa"); query t(v); TEST_ASSERT(v == t && !(v < t));}
    {
        query v; string s("a=1&b=%20&#"); string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.encoding() == "a=1&b=%20" && *it == '#');
    } {
        query v; string s("a=1&b=%20&d#"); string::const_iterator it = s.begin(); string errs;
        TEST_ASSERT(parse(it, s.end(), v, &errs) && v.encoding() == "a=1&b=%20" && *it == '#');
        TEST_ASSERT(errs == "URI query: \"a=1&b=%20&d\", (11) missing \"=\" (ignoring key)");
    } {
        query v; string s("a=1&b=%20&d&c=%#"); string::const_iterator it = s.begin(); string errs;
        TEST_ASSERT(parse(it, s.end(), v, &errs) && v.encoding() == "a=1&b=%20&c=%25" && *it == '#');
        TEST_ASSERT(!errs.empty());
    }
}
static void test_fragment() {
    /// sub-delims = "!" | "$" | "&" | "'" | "(" | ")" | "*"
    ///            | "+" | "," | ";" | "="
    /// unreserved = ALPHA | DIGIT | "-" | "." | "_" | "~"
    /// pchar = unreserved | pct-encoded | sub-delims | ":" | "@"
    /// fragment = *( pchar | "/" | "?" )
    {fragment v; TEST_ASSERT(v.empty());}
    {fragment v(""); TEST_ASSERT(v.empty());}
    {fragment v("a"); TEST_ASSERT(v.string() == "a" && v.encoding() == "a");}
    {fragment v("%25"); TEST_ASSERT(v.string() == "%" && v.encoding() == "%25");}
    {fragment v("!$&'()*+,;=-._~:@/?"); TEST_ASSERT(v.string() == "!$&'()*+,;=-._~:@/?" && v.encoding() == "!$&'()*+,;=-._~:@/?");}
    {
        fragment v; string s("% "); string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v) && *it == '%');
    } {
        fragment v; string s("% "); string::const_iterator it = s.begin(); string errs;
        TEST_ASSERT(parse(it, s.end(), v, &errs) && v.encoding() == "%25" && *it == ' ' && !errs.empty());
    } {
        fragment v; string s("!$&'()*+,;=-._~:@/?<"); string::const_iterator it = s.begin(); string errs;
        TEST_ASSERT(parse(it, s.end(), v, &errs) && v.string() == "!$&'()*+,;=-._~:@/?" && *it == '<' && errs.empty());
    }
}
static void test_uri() {
    {uri v; TEST_ASSERT(v.is_null());}
    {uri v("#!$&'()*+,;=-._~:@/?"); TEST_ASSERT(!v.is_null() && v.encoding() == "#!$&'()*+,;=-._~:@/?" && v.relative());}
    {uri v("?a=1&b=%20&c=0#foo"); TEST_ASSERT(v.encoding() == "?a=1&b=%20&c=0#foo" && v.relative() && v.fragment().string() == "foo");}
    {uri v("foo:"); TEST_ASSERT(v.encoding() == "foo:" && v.scheme().string() == "foo" && !v.relative());}
    {uri v("foo:#"); TEST_ASSERT(v.encoding() == "foo:" && v.scheme().string() == "foo" && !v.relative());}
    {uri v("abc"); TEST_ASSERT(v.encoding() == "abc" && v.path().encoding() == "abc" && v.relative());}
    {uri v("//[1::0]"); TEST_ASSERT(v.encoding() == "//[1::0]" && v.authority().host() == "[1::0]" && v.relative());}
    {
        uri v; string s("abc "); string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.encoding() == "abc" && v.path().encoding() == "abc" &&
            v.relative() && *it == ' ');
    } {
        uri v; string s("%#% "); string::const_iterator it = s.begin(); string errs;
        TEST_ASSERT(parse(it, s.end(), v, &errs) && v.encoding() == "%25#%25" && v.path().encoding() == "%25" &&
            v.fragment().encoding() == "%25" && v.relative() && *it == ' ' && !errs.empty());
    } {
        uri v("http://woo.com?a=1&b=%20&c=0#foo");
        TEST_ASSERT(v.encoding() == "http://woo.com?a=1&b=%20&c=0#foo" && v.fragment().string() == "foo" &&
            v.scheme().string() == "http" && v.authority().host() == "woo.com" && v.path().empty() &&
            v.query().encoding() == "a=1&b=%20&c=0");
    } {
        uri v("http://woo.com/?a=1&b=%20&c=0#foo");
        TEST_ASSERT(v.encoding() == "http://woo.com/?a=1&b=%20&c=0#foo" && v.fragment().string() == "foo" &&
            v.scheme().string() == "http" && v.authority().host() == "woo.com" && v.path().absolute() &&
            v.query().encoding() == "a=1&b=%20&c=0");
    } {
        uri v("http://woo.com:440/x/y/z?a=1&b=%20&c=0#foo");
        TEST_ASSERT(v.encoding() == "http://woo.com:440/x/y/z?a=1&b=%20&c=0#foo" && v.fragment().string() == "foo" &&
            v.scheme().string() == "http" && v.authority().host() == "woo.com" && v.path().absolute() &&
            v.path().encoding() == "/x/y/z" && v.query().encoding() == "a=1&b=%20&c=0");
    }
    try {uri v("http://woo.com:440/%"); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    { // example in uri.h
    	uri u("http://mydomain.com/a/b/c?id=12345&s=x%20y");
    	assert(u.scheme().string() == "http");
    	assert(u.authority().host() == "mydomain.com");
    	assert(u.path().absolute());
    	path::const_iterator it = u.path().begin();
    	assert(*it == "a" && *++it == "b" && *++it == "c" && ++it == u.path().end());
    	int id;
    	std::string s;
    	bool is_null;
    	assert(u.query().find("id", id, is_null) && id == 12345 && !is_null);
    	assert(u.query().find("s", s, is_null) && s == "x y" && !is_null);
    	assert(!u.query().find("foo", s, is_null));
    }
}
namespace uripp_test {
    void uri_tests(test_utils::test& t) {
        t.add("uri decode", test_decode);
        t.add("uri scheme", test_scheme);
        t.add("uri authority", test_authority);
        t.add("uri path", test_path);
        t.add("uri query", test_query);
        t.add("uri fragment", test_fragment);
        t.add("uri uri", test_uri);
    }
}
