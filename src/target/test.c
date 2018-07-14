#include "munit.h"

#include "unicode.h"

struct u_pair {
    char    *utf8;
    wchar_t *utf32;
};

struct u_pair text_fixture[] = {
    {
        .utf8  =  "Single byte ascii symbols string",
        .utf32 = L"Single byte ascii symbols string"
    },
    {
        .utf8  =  "\u041A\u0438\u0440\u0438\u043B\u043B\u0438\u0446\u0430",
        .utf32 = L"\u041A\u0438\u0440\u0438\u043B\u043B\u0438\u0446\u0430"
    },
    {
        .utf8  =  "\u0814\u0820\u080C\u081D\u0813\u0829\u0809\u080C",
        .utf32 = L"\u0814\u0820\u080C\u081D\u0813\u0829\u0809\u080C"
    },
    {
        .utf8  =  "𓁹",
        .utf32 = L"𓁹"
    }
};


static void *test_utf8_strlen_setup(const MunitParameter params[], void *user_data) {
    (void) params;
    (void) user_data;

    return text_fixture;
}

static MunitResult test_utf8_strlen(const MunitParameter params[], void *fixture) {
    (void) params;
    struct u_pair *f = fixture;

    munit_assert_ulong(utf8_strlen(f[0].utf8), ==, strlen(f[0].utf8));
    munit_assert_ulong(utf8_strlen(f[1].utf8) * 2, ==, strlen(f[1].utf8));
    munit_assert_ulong(utf8_strlen(f[2].utf8) * 3, ==, strlen(f[2].utf8));
    munit_assert_ulong(utf8_strlen(f[3].utf8) * 4, ==, strlen(f[3].utf8));

    return MUNIT_OK;
}

static const MunitSuite test_suite = {
    "unit",
    (MunitTest[]){
        {
            "/unicode/utf8_strlen",
            test_utf8_strlen,
            test_utf8_strlen_setup,
            NULL,
            MUNIT_TEST_OPTION_NONE,
            NULL
        },
        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
    },
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char **argv) {
    return munit_suite_main(&test_suite, NULL, argc, argv);
}
