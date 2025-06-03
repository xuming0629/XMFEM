#include <iostream>
#include <cassert>

/**
 * @brief 简单的加法函数
 */
int add(int a, int b) {
    return a + b;
}

/**
 * @brief 测试 add 函数是否按预期工作
 * 
 * @return 若所有测试通过返回 true，否则返回 false
 */
bool testAdd() {
    // 1) 正常两个正数相加
    if (add(2, 3) != 5) return false;

    // 2) 正数与零相加
    if (add(7, 0) != 7) return false;

    // 3) 负数与正数相加
    if (add(-4, 6) != 2) return false;

    // 4) 两个负数相加
    if (add(-5, -3) != -8) return false;

    // 5) 边界值示例：大数相加（不会溢出的范围内）
    if (add(1000000, 2000000) != 3000000) return false;

    return true;
}

int main() {
    if (testAdd()) {
        std::cout << "[Add Function Test] PASS\n";
        return 0;
    } else {
        std::cerr << "[Add Function Test] FAIL\n";
        return 1;
    }
}
