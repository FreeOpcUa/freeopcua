#include "test.h"
#include <iostream>
namespace uripp_test {
    void utils_tests(test_utils::test& t);
    void ipsyn_tests(test_utils::test& t);
    void uri_tests(test_utils::test& t);
    inline void setup(test_utils::test& t) {
	    utils_tests(t);
	    ipsyn_tests(t);
	    uri_tests(t);
    }
}
int main(int argc, char** argv) {
    test_utils::test test;
    uripp_test::setup(test);
    int res = test.run();
    if (res)
        std::cerr << "*** " << test_utils::test::errors() << " test(s) failed" << std::endl;
    else
        std::cerr << "--- no errors" << std::endl;
    return res;
}

