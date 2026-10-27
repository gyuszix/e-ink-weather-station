// Minimal assertion helpers; no external test framework needed.
#ifndef TEST_H
#define TEST_H

#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>

static int testFailures = 0;
static int testChecks   = 0;

#define CHECK(cond) do { testChecks++; if (!(cond)) { testFailures++; \
  printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)
#define CHECK_STR(a, b) do { testChecks++; if (strcmp((a), (b)) != 0) { testFailures++; \
  printf("  FAIL %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, (a), (b)); } } while (0)
#define CHECK_NEAR(a, b, tol) do { testChecks++; if (std::fabs((a) - (b)) > (tol)) { testFailures++; \
  printf("  FAIL %s:%d: %s = %g, expected %g\n", __FILE__, __LINE__, #a, (double)(a), (double)(b)); } } while (0)

#define TEST(name) static void name(); struct name##_reg { name##_reg() { \
  testRegistry()[testCount()++] = {#name, name}; } } name##_inst; static void name()

struct TestCase { const char *name; void (*fn)(); };
static TestCase *testRegistry() { static TestCase r[256]; return r; }
static int &testCount() { static int n = 0; return n; }

static int runAllTests() {
  for (int i = 0; i < testCount(); i++) {
    int before = testFailures;
    testRegistry()[i].fn();
    printf("%s %s\n", testFailures == before ? "ok  " : "FAIL", testRegistry()[i].name);
  }
  printf("\n%d checks, %d failed\n", testChecks, testFailures);
  return testFailures == 0 ? 0 : 1;
}

static String readFile(const std::string &path) {
  std::ifstream f(path);
  std::stringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

#endif
