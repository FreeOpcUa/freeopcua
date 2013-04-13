#include "test.h"
#include "../src/utils.h"
#include <iostream>
#include <stdexcept>
using namespace std;
using namespace uripp;
static void test_convert() {
    TEST_ASSERT(convert(true) == "1" && convert(false) == "0");
    TEST_ASSERT(convert(0) == "0");
    TEST_ASSERT(convert(2147483647) == "2147483647");
    {int v = -2147483647; --v; TEST_ASSERT(convert(v) == "-2147483648");}
    {unsigned int v = 0; TEST_ASSERT(convert(v) == "0");}
    {unsigned int v = 4294967295; TEST_ASSERT(convert(v) == "4294967295");}
    {long long v = 0; TEST_ASSERT(convert(v) == "0");}
    {long long v = 9223372036854775807LL; TEST_ASSERT(convert(v) == "9223372036854775807");}
    {long long v = -9223372036854775807LL; --v; TEST_ASSERT(convert(v) == "-9223372036854775808");}
    {unsigned long long v = 0; TEST_ASSERT(convert(v) == "0");}
    {unsigned long long v = 18446744073709551615ULL; TEST_ASSERT(convert(v) == "18446744073709551615");}
    TEST_ASSERT(convert((double)0) == "0");
#	ifdef _WIN32
    TEST_ASSERT(convert((double)10000000) == "1e+007");
    TEST_ASSERT(convert((double).00000001) == "1e-008");
    TEST_ASSERT(convert((double)-.00000001) == "-1e-008");
#	else
    TEST_ASSERT(convert((double)10000000) == "1e+07");
    TEST_ASSERT(convert((double).00000001) == "1e-08");
    TEST_ASSERT(convert((double)-.00000001) == "-1e-08");
#	endif    
    {bool v; TEST_ASSERT(!convert("", v));}
    {bool v; TEST_ASSERT(!convert("\t\t\t\n", v));}
    {bool v; TEST_ASSERT(convert("1", v) && v);}
    {bool v; TEST_ASSERT(convert("    1     ", v) && v);}
    {bool v; TEST_ASSERT(convert("on", v) && v);}
    {bool v; TEST_ASSERT(convert("FALSE", v) && !v);}
    try {bool v; convert("foo", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {bool v; convert("1      a", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    {int v; TEST_ASSERT(!convert("", v));}
    {int v; TEST_ASSERT(!convert("    \t\n", v));}
    {int v = 9; TEST_ASSERT(convert("    \t\n0 ", v) && v == 0);}
    {int v; TEST_ASSERT(convert("1", v) && v == 1);}
    {int v; TEST_ASSERT(convert("    123     ", v) && v == 123);}
    {int v; TEST_ASSERT(convert("    -93344455", v) && v == -93344455);}
    {int v; TEST_ASSERT(convert("2147483647", v) && v == 2147483647);}
    {int v; TEST_ASSERT(convert("-2147483648", v) && ++v == -2147483647);}
    try {int v; convert("2147483648", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {int v; convert("-2147483649", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {int v; convert("foo", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {int v; convert("1      a", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    {unsigned int v; TEST_ASSERT(!convert("", v));}
    {unsigned int v; TEST_ASSERT(!convert("    \t\n", v));}
    {unsigned int v; TEST_ASSERT(convert("1", v) && v == 1);}
    {unsigned int v; TEST_ASSERT(convert("    4294967295     ", v) && v == 4294967295);}
    try {unsigned int v; convert("4294967296", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {unsigned int v; convert("foo", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {unsigned int v; convert("1      a", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    {long long v; TEST_ASSERT(!convert("", v));}
    {long long v; TEST_ASSERT(!convert("    \t\n", v));}
    {long long v; TEST_ASSERT(convert("1", v) && v == 1);}
    {long long v; TEST_ASSERT(convert("    123     ", v) && v == 123LL);}
    {long long v; TEST_ASSERT(convert("    -93344455", v) && v == -93344455LL);}
    {long long v; TEST_ASSERT(convert("9223372036854775807", v) && v == 9223372036854775807LL);}
    {long long v; TEST_ASSERT(convert("-9223372036854775808", v) && ++v == -9223372036854775807LL);}
    try {long long v; convert("9223372036854775808", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {long long v; convert("-9223372036854775809", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {long long v; convert("foo", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {long long v; convert("1      a", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    {unsigned long long int v; TEST_ASSERT(!convert("", v));}
    {unsigned long long int v; TEST_ASSERT(!convert("    \t\n", v));}
    {unsigned long long int v = 9; TEST_ASSERT(convert("    \t\n0 ", v) && v == 0);}
    {unsigned long long int v; TEST_ASSERT(convert("1", v) && v == 1);}
    {unsigned long long int v; TEST_ASSERT(convert("    18446744073709551615     ", v) && v == 18446744073709551615ULL);}
    try {unsigned long long int v; convert("18446744073709551616", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {unsigned long long int v; convert("foo", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {unsigned long long int v; convert("1      a", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    {double v; TEST_ASSERT(!convert("", v));}
    {double v; TEST_ASSERT(!convert("    \t\n", v));}
    {double v = 9; TEST_ASSERT(convert("    \t\n0 ", v) && v == 0.0);}
    {double v; TEST_ASSERT(convert("1", v) && v == 1.0);}
    {double v; TEST_ASSERT(convert("    1e-003     ", v) && v == 1e-003);}
    {double v; TEST_ASSERT(convert("    -93344455", v) && v == -93344455.0);}
    try {double v; convert("foo", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
    try {double v; convert("1      a", v); TEST_ASSERT(false);} catch (const std::invalid_argument&e) {(void)e;}
}
namespace uripp_test {
    void utils_tests(test_utils::test& t) {
        t.add("utils convert", test_convert);
    }
}
