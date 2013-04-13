#include "test.h"
#include <iostream>
namespace test_utils {
    int test::errors_ = 0;
    void test::assertFailure(const std::string& msg, int line) {
        throw exception(msg, line);
    }
    void test::add(const std::string& name, case_type c) {cases_.push_back(std::make_pair(name, c));}
    int test::run() {
        for (cases_type::iterator it = cases_.begin(); it != cases_.end(); ++it) {
            std::cout << "test: " << it->first << std::endl;
            bool ok = false;
            try {
                (*it->second)();
                ok = true;
            } catch (const exception& e) {
                std::cout << "*** Failure(" << e.line() << "): " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cout << "*** Failure: unexpected: " << e.what() << std::endl;
            } catch (...) {
                std::cout << "*** Failure: unknown exception" << std::endl;
            }
            if (!ok)
                ++errors_;
        }
        return errors_ ? 1 : 0;
    }
}
