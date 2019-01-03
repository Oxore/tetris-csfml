#include "munit.h"

static void *test_dummy_setup(
        const MunitParameter    params[],
        void                   *user_data)
{
    (void) params;

    return user_data;
}

static MunitResult test_dummy(
        const MunitParameter    params[],
        void                   *fixture)
{
    (void) params;
    (void) fixture;

    return MUNIT_OK;
}

static const MunitSuite test_suite = {
    "unit",
    (MunitTest[]){
        {
            "/dummy/dummy",
            test_dummy,
            test_dummy_setup,
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

int main(int argc, char **argv)
{
    return munit_suite_main(&test_suite, NULL, argc, argv);
}
