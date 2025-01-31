/*
 * Copyright (c) 2019 by Red Hat, Inc.
 *
 * Author: Anderson Toshiyuki Sasaki <ansasaki@redhat.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

static void test_skip1(void **state)
{
    (void)state;

    assert_true(1);
}

static void test_skip2(void **state)
{
    (void)state;

    assert_false(1);
}

static void test_fail(void **state)
{
    (void)state;

    assert_false(1);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_skip1),
        cmocka_unit_test(test_skip2),
        cmocka_unit_test(test_fail),
    };

    cmocka_set_test_filter("test_skip*");

    cmocka_set_skip_filter("test_skip2");

    return cmocka_run_group_tests(tests, NULL, NULL);
}
