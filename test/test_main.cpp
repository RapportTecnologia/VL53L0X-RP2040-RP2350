#include "unity.h"

// Exemplo de teste de unidade
void test_dummy(void) {
    TEST_ASSERT_EQUAL(1, 1);
}

// Unity requires these functions
void setUp(void) {
    // set up operations (if any)
}

void tearDown(void) {
    // clean up operations (if any)
}

// Função principal que executa os testes
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy);
    return UNITY_END();
}
