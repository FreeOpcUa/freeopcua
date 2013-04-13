#ifndef test_h
#define test_h
#include <string>
#include <vector>
#include <utility>
#include <exception>
#define TEST_ASSERT(t) (t) ? (void)0 : test_utils::test::assertFailure(#t, __LINE__)
namespace test_utils {
    /// Test class.
    /// \code
    /// #include "test.h"
    /// namespace foo_test {
    ///     void test_bar() {
    ///         ...
    ///         TEST_ASSERT(a == b);
    ///         ...
    ///     }
    ///     void bar_tests(test_utils::test& t) {
    ///         t.add("bar", test_bar);
    ///     }
    /// }
    /// int _tmain(int argc, _TCHAR* argv[]) {
    ///     test_utils::test test;
    ///     foo_test::bar_tests(test);
    ///     return test.run();
    /// }
    /// \endcode
    class test {
    public:
        test() {} ///< Construct.
        typedef void (*case_type)();
        /// Add case to test.
        void add(const std::string& name, case_type c);
        /// Run and return exit code.
        int run();
        static int errors() {return errors_;} ///< number of errors
        /// Exception class.
        class exception : public std::exception {
        public:
            exception(const std::string& msg, int line) throw () : msg_(msg), line_(line) {} ///< Construct.
            ~exception() throw () {}
            const char* what() const throw () {return msg_.c_str();} ///< what?
            int line() const {return line_;} ///< line
        private:
            std::string msg_;
            int line_;
        };
        /// Throw exception and bump error count.
        static void assertFailure(const std::string& msg, int line);
    private:
        typedef std::vector<std::pair<std::string, case_type> > cases_type;
        cases_type cases_;
        static int errors_;
    };
}
#endif
