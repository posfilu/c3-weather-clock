/*
 * 测试框架自检：确认 Unity、CTest 和 sanitizer 都接好了。
 * 等第一个 *_core 组件有了真正的测试，这个文件就可以删掉。
 */
#include <string.h>

#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

static void test_unity_is_wired_up(void)
{
    TEST_ASSERT_EQUAL_INT(4, 2 + 2);
}

static void test_heap_roundtrip_under_asan(void)
{
    char buf[8];
    memset(buf, 'x', sizeof(buf));
    TEST_ASSERT_EACH_EQUAL_CHAR('x', buf, sizeof(buf));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_unity_is_wired_up);
    RUN_TEST(test_heap_roundtrip_under_asan);
    return UNITY_END();
}
