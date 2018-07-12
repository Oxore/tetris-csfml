#include "munit.h"

#include "unicode.h"

struct u_pair {
    char    *utf8;
    wchar_t *utf32;
};

struct u_pair *text_fixture;

static void *test_text_setup(const MunitParameter params[], void *user_data) {
    (void) params;
    (void) user_data;

    return text_fixture;
}

static MunitResult test_text(const MunitParameter params[], void *fixture) {
    (void) params;
    struct u_pair *f = fixture;

    munit_assert_ulong(utf8_strlen(f[0].utf8), ==, strlen(f[0].utf8));

    return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
    {
        "/example/parameters",
        test_text,
        test_text_setup,
        NULL,
        MUNIT_TEST_OPTION_NONE,
        NULL
    },
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

static const MunitSuite test_suite = {
    "unit",
    test_suite_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char **argv) {
    text_fixture = (struct u_pair []){
        {
            .utf8 = "Single byte ascii symbols string",
            .utf32 = L"Single byte ascii symbols string"
        }
    };

    return munit_suite_main(&test_suite, NULL, argc, argv);
}
