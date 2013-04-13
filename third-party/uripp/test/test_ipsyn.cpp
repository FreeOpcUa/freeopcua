#include "test.h"
#include "../src/ip_address.h"
#include "../src/ipv6_address.h"
#include "../src/domain_name.h"
#include <iostream>
#include <stdexcept>
using namespace std;
using namespace uripp;
static void test_ip() {
    {
        string s("0.0.0.0"); ip_address v;
        std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "0.0.0.0" && it == s.end());
    } {
        string s("0.0.0.99 "); ip_address v;
        std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "0.0.0.99" && *it == ' ');
    } {
        string s("255.0.0.255."); ip_address v;
        std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "255.0.0.255" && *it == '.');
    } {
        string s("255.0.0.256."); ip_address v;
        std::string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v) && it == s.begin());
    } {
        string s("255.0.0."); ip_address v;
        std::string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v) && it == s.begin());
    }
    {ip_address v; TEST_ASSERT(v.is_null());}
    {ip_address v("0.0.0.0"); TEST_ASSERT(v.is_null());}
    {ip_address v("0.0.0.1"); TEST_ASSERT(v.string() == "0.0.0.1");}
    {ip_address v("255.99.255.1"); TEST_ASSERT(v.string() == "255.99.255.1");}
    try {ip_address v(""); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    try {ip_address v("255.0.0"); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
}
static void test_ipv6() {
    {ipv6_address v; TEST_ASSERT(v.is_null());}
    {
        string s("0:0:0:0:0:0:0:0"); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.is_null() && it == s.end());
    } {
        string s("0:0:0:0:0:0::0"); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.is_null() && it == s.end());
    } {
        string s("0:0:0:0:0::0"); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.is_null() && it == s.end());
    } {
        string s("0::0"); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.is_null() && it == s.end());
    } {
        string s("::0"); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.is_null() && it == s.end());
    } {
        string s("::"); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.is_null() && v.string(true) == "::" && it == s.end());
    } {
        string s("::"); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.is_null() && v.string() == "0:0:0:0:0:0:0:0" && it == s.end());
    } {
        string s("::1."); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string(true) == "::1" && *it == '.');
    } {
        string s("::1."); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "0:0:0:0:0:0:0:1" && *it == '.');
    } {
        string s("1:F:FF:FFF:9Fe:0801:F001:Ffff "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "1:F:FF:FFF:9FE:801:F001:FFFF" && *it == ' ');
    } {
        string s("1:F:FF:FFF:9Fe:0801:F0010:Ffff "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v) && v.is_null() && it == s.begin());
    } {
        string s("1:F::FFF::0801:F0010:Ffff "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v) && v.is_null() && it == s.begin());
    } {
        string s("1:F:FF:FFF:9Fe:::Ffff "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v) && v.is_null() && it == s.begin());
    } {
        string s("1:F:FF:FFF:9Fe::F001:Ffff "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string(true) == "1:F:FF:FFF:9FE::F001:FFFF" && *it == ' ');
    } {
        string s("1:F:FF::F001:Ffff "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string(true) == "1:F:FF::F001:FFFF" && *it == ' ');
    } 
    {ipv6_address v("1:F:FF:FFF:9Fe:0801:F001:Ffff"); TEST_ASSERT(v.string() == "1:F:FF:FFF:9FE:801:F001:FFFF");}
    {ipv6_address v("::"); TEST_ASSERT(v.is_null());}
    {ipv6_address v("::1"); TEST_ASSERT(v.string(true) == "::1");}
    {ipv6_address v("ffff::"); TEST_ASSERT(v.string(true) == "FFFF::");}
    try {ipv6_address v(""); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    try {ipv6_address v("1:F:FF:FFF:9Fe:::Ffff"); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    {
        string s("1:F:FF:FFF:9Fe:0801:13.1.68.3 "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "1:F:FF:FFF:9FE:801:D01:4403" && *it == ' ');
    } {
        string s("1:F:FF:FFF:9Fe::13.1.68.3 "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string(true) == "1:F:FF:FFF:9FE::D01:4403" && *it == ' ');
    } {
        string s("1::13.1.68.3 "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string(true) == "1::D01:4403" && *it == ' ');
    } {
        string s("::13.1.68.3."); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string(true) == "::D01:4403" && *it == '.');
    } { // misplaced v4 but still parses as v6
        string s("1:F:FF:FFF:9Fe:0801:3:13.1.68.3 "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "1:F:FF:FFF:9FE:801:3:13" && *it == '.');
    } {
        string s("1:F:FF:FFF:9Fe:13.1.68.3 "); ipv6_address v; std::string::const_iterator it = s.begin();
        TEST_ASSERT(!parse(it, s.end(), v) && v.is_null() && it == s.begin());
    } 
}
static void test_dn() {
    {domain_name v; TEST_ASSERT(v.is_null());}
    {domain_name v; TEST_ASSERT((v + domain_name()).is_null());}
    {domain_name v("A"); TEST_ASSERT(v.string() == "a");}
    {domain_name v("9"); TEST_ASSERT(v.string() == "9");}
    {domain_name v("A.b"); TEST_ASSERT(v.string() == "a.b");}
    {domain_name v("A------1"); TEST_ASSERT(v.string() == "a------1");}
    {domain_name v("A-1.b2"); TEST_ASSERT(v.string() == "a-1.b2");}
    {domain_name v("A-1.b2.c"); TEST_ASSERT(v.string() == "a-1.b2.c");}
    {domain_name v("0A-1.b2.c"); TEST_ASSERT(v.string() == "0a-1.b2.c");}
    {domain_name v("0A-1.b2.c"); TEST_ASSERT((v + string("z")).string() == "0a-1.b2.c.z");}
    {domain_name v("0A-1.b2.c"); TEST_ASSERT((v + domain_name()).string() == "0a-1.b2.c");}
    {
        string s("A--- "); domain_name v;
        std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "a" && *it == '-' && *++it == '-' && *++it == '-');
    } {
        string s("A."); domain_name v;
        std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "a" && *it == '.');
    } {
        string s("A-b-. "); domain_name v;
        std::string::const_iterator it = s.begin();
        TEST_ASSERT(parse(it, s.end(), v) && v.string() == "a-b" && *it == '-');
    }
    try {domain_name v(""); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    try {domain_name v("a-"); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
    try {domain_name v("a,"); TEST_ASSERT(false);} catch (const invalid_argument&e) {(void)e;}
}
namespace uripp_test {
    void ipsyn_tests(test_utils::test& t) {
        t.add("ipsyn ip", test_ip);
        t.add("ipsyn ipv6", test_ipv6);
        t.add("ipsyn dn", test_dn);
    }
}
