# AI 出题

## 一、C/C++ 语言基础

### 1. **数据类型与运算符**

**题目1**：在32位系统中，表达式 `sizeof('A') + sizeof(3.14f)`的结果是？

A) 4 

B) 5 

C) 8 

D) 9

**答案**：B（`'A'`是char类型占1字节，`3.14f`是float占4字节）

------

**题目2**：寄存器操作时，需要将GPIO的bit5置1，其他位保持不变，应使用：

A) `reg |= 0x20;`

B) `reg &= 0x20;`

C) `reg = 0x20;`

D) `reg ^= 0x20;`

**答案**：A（`0x20`对应二进制 `0010 0000`）

------

**题目3**：表达式 `(uint8_t)300 + 1`的结果是？

A) 301 

B) 45 

C) 44 

D) 0

**答案**：B（300截断为44，44+1=45）

------

**题目4**：关于浮点数精度，错误的是：

A) `0.1f + 0.2f == 0.3f`可能为假

B) 嵌入式系统常用单精度节省内存

C) `float`可精确表示所有整数

D) 比较浮点数需用 `fabs(a-b)<epsilon`

**答案**：C（超出16777216的整数无法精确表示）

------

**题目5**：下列代码的输出是？

```c
int a = 5, b = 3;
printf("%d", a++ * --b);
```

A) 15 B) 10 C) 12 D) 8

**答案**：B（`a=5, b=2, 5 * 2=10`）

------

**题目6**：位域声明 `struct { unsigned flag:1; }`中，`flag`的取值范围是：

A) 0 或 1

B) -1 或 0

C) 0~255

D) 由编译器决定

**答案**：A（1位无符号位域仅能表示0或1）

------

**题目7**：表达式 `~0x0F`的十六进制结果是：

A) 0xF0 B) 0x0F C) 0xFF D) 0xFFFF

**答案**：A（`0x0F`二进制 `0000 1111`，取反得 `1111 0000=0xF0`）

-------

**题目8**：在STM32 HAL库中，用于检查寄存器某位是否为1的宏通常是：

A) `READ_BIT(reg, bit)`

B) `SET_BIT(reg, bit)`

C) `CLEAR_BIT(reg, bit)`

D) `(reg) |= (bit)`

**答案**：A

------

**题目9**：STM32 上下列代码的输出是？

```c
uint16_t val = 0x1234;
printf("%d", *(uint8_t*)&val);
```

A) 0x12 

B) 0x34

C) 18 

D) 52

**答案**：D（`0x34`，低地址存储低字节）

------

**题目10**：以下哪种整型转换可能丢失精度？

A) `int32_t`→ `int64_t`

B) `uint8_t`→ `int16_t`

C) `float`→ `int`

D) `uint16_t`→ `uint8_t`

**答案**：D（超出255的值丢失高字节）

------

**题目11**：表达式 `0x80 >> 2`的结果是？

A) 0x40

B) 0x20

C) 0x10

D) 0x08

**答案**：B

-------

**题目12**：关于枚举类型，正确的是：

A) `enum {A=5, B};`中B值为6

B) 枚举常量占用2字节内存

C) 枚举值可赋浮点数

D) 嵌入式系统优先使用enum而非#define

**答案**：A（枚举自动递增）

------

##### **题目13**：下列代码的输出是？

```c
int x = 7, y = -7;
printf("%d %d", x >> 1, y >> 1);
```

A) 3 -3 

B) 3 -4 

C) 3 无定义 

D) 编译错误

**答案**：B（负数的右移是**实现定义**行为，GCC/ARM中 -7>>1 = -4）

**解析**：-7二进制补码：`1111 1001`>>1 = `1111 1100`（-4）

------

##### **题目14**：避免浮点数运算的开销，嵌入式系统处理小数时常用：

A) 定点数（如Q格式）

B) double类型提升精度

C) 软件浮点库

D) 拒绝使用小数

**答案**：A（如Q15格式表示小数）

------

**题目15**：表达式 `(uint16_t)0xFFFF + 1`的结果是？

A) 0x10000 

B) 0x0000 

C) 0xFFFF 

D) 溢出错误

**答案**：B（无符号整型溢出回绕为0）

### 2. 控制结构

**题目1**：下列代码中，循环体执行多少次？

```c
for(int i=0; i<10; i++) {
    if(i % 3 == 0) continue;
    printf("%d", i);
}
```

A) 6次 

B) 7次 

C) 9次 

D) 10次

**答案**：A（输出1,2,4,5,7,8，跳过0,3,6,9）

------

**题目2**：优化嵌入式实时系统时，优先减少哪类循环开销？

A) 固定次数的for循环

B) 基于状态机的while循环

C) 递归调用

D) 嵌套循环最内层

**答案**：D（最内层循环执行次数最多，优化收益最大）

------

**题目3**：下列代码的输出是？

```c
int x = 2;
switch(x) {
    case 1: x += 1;
    case 2: x += 2;
    case 3: x += 3;
    default: x += 4;
}
printf("%d", x);
```

A) 2 

B) 4 

C) 7 

D) 11

**答案**：D（case 2后无break，贯穿执行到default：2+2+3+4=11）

------

**题目4**：在MCU中断服务函数中应避免：

A) 使用while循环等待标志位

B) 调用带互斥锁的函数

C) 操作volatile变量

D) 直接修改寄存器

**答案**：A（可能阻塞中断导致系统崩溃）

------

##### **题目5**：下列代码中，哪个分支能加速32位整数的符号判断？

```c
int32_t val = sensor_value;
// 选项：
A) if(val > 0) {...} 
B) if(val & 0x80000000) {...} 
C) if(val >> 31) {...} 
D) if(val < 0) {...}
```

**答案**：B（位运算比比较指令更快，尤其无FPU时）

------

**题目6**：下列代码的时间复杂度是？

```c
for(int i=0; i<n; i+=2) {
    process_data(i);
}
```

A) O(1) 

B) O(log n) 

C) O(n) 

D) O(n²)

**答案**：C（循环次数 n/2 → 仍为线性阶）

------

**题目7**：以下状态机实现错误的是？

```c
enum State {IDLE, RUN, ERROR} state = IDLE;
while(1) {
    switch(state) {
        case IDLE:
            if(start_signal) state = RUN;
            break;
        case RUN:
            if(error_flag) state = ERROR;
        case ERROR:
            handle_error();
            state = IDLE;
    }
}
```

A) IDLE分支无错误

B) RUN分支缺少break导致错误

C) ERROR分支不应重置状态

D) enum内存占用过大

**答案**：B（RUN分支执行后会立即执行ERROR分支）

------

**题目8**：在无人机飞控系统中，需要快速计算一组传感器的均值，原始代码如下：

```c
float sensor_values[8];
float sum = 0.0f;
for(int i = 0; i < sizeof(sensor_values)/sizeof(sensor_values[0]); i++) {
    sum += sensor_values[i]; 
}
float avg = sum / 8.0f;
```

以下哪种优化方式**最能提高实时性**？

A) 使用定点数代替浮点数

B) 将循环展开为顺序加法

C) 将除数改为乘法（`*0.125f`）

D) 预先计算数组长度 `int len = sizeof(sensor_values)/sizeof(sensor_values[0])`

**答案**：B（将循环展开为顺序加法）

------

**题目9**：在姿态控制模块中，需根据飞行状态选择控制算法：

```c
switch(flight_mode) {
    case TAKEOFF:  run_takeoff(); break;
    case HOVER:    run_hover();   break;
    case LANDING:  run_land();    break;
    default:       run_fallback();
}
```

若测试数据表明80%情况处于HOVER模式，如何优化？

A) 将HOVER分支移到switch开头

B) 用if-else链代替switch

C) 增加`[[likely]]`属性提示编译器

D) 使用函数指针数组

**答案**：**D**（使用函数指针数组）

------

**题目10**：下列代码执行后，变量`count`的值是？

```c
uint8_t count = 0;
uint8_t i = 255;
while(i++ < 200) {
    count++;
}
```

A) 45 

B) 200 

C) 0 

D) 1

**答案**：C

### 3. 预处理器与宏

**题目1**：下列宏定义中，哪个能安全计算数组长度？

```c
#define ARR_LEN(arr) ?
```

A) `sizeof(arr) / sizeof(arr[0])`

B) `sizeof(arr) / sizeof(*arr)`

C) `sizeof(arr) / sizeof(int)`

D) A和B均可

**答案**：D（A和B等价，均可获得元素个数）

------

**题目2**：条件编译指令 `#if defined(FLIGHT_CONTROL)`的等效写法是？

A) `#ifdef FLIGHT_CONTROL`

B) `#if defined FLIGHT_CONTROL`

C) `#if FLIGHT_CONTROL == 1`

D) `#ifndef FLIGHT_CONTROL`

**答案**：A

------

**题目3**：在STM32 HAL库开发中，`__HAL_RCC_GPIOA_CLK_ENABLE()`宏的作用机制是？

A) 函数调用

B) 直接操作寄存器地址

C) 内联汇编

D) 编译器指令

**答案**：B（HAL库使用地址映射：`RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN`）

------

**题目4**：下列代码输出？

```
#define POWER(x)  x * x  
printf("%d", POWER(3 + 2));
```

A) 11 

B) 25 

C) 编译错误 

D) 不确定

**答案**：A（宏展开为 `3 + 2 * 3 + 2 = 3+6+2=11`）

------

**题目5**：跨平台代码中，检测编译器是否为GCC的宏是？

A) `#ifdef __GNUC__`

B) `#if __GCC__`

C) `#if _MSC_VER`

D) `#if __ARMCC__`

**答案**：A（`__GNUC__`是GCC/Clang的预定义宏）

------

**题目6**：`#pragma pack(push, 1)`的作用是？

A) 保存当前对齐方式并设置1字节对齐

B) 弹出堆栈中的对齐设置

C) 内存页面对齐

D) 设置代码段位置

**答案**：A（`#pragma pack(push/pop)`用于协议解析时的内存压缩）

------

**题目7**：下列哪个宏可防止头文件重复包含？

A) `#pragma once`

B) `#ifndef _HEADER_H_`

C) `#define _HEADER_H_ ... #endif`

D) 以上都是

**答案**：D（A是编译器特性，B/C是C标准方案）

------

**题目8**：关于 `##`运算符，正确的是？

```
#define CONNECT(a,b) a##b  
int x = CONNECT(12, 34);
```

A) `x=46`

B) `x=1234`

C) `x=0`

D) 编译错误

**答案**：B（`##`拼接标记为 `1234`）

**用途**：生成函数名如 `UART##_Init()`

------

**题目9**：下列代码的输出？

```c
#define DEBUG 0  
#if DEBUG  
    printf("LOG: Sensor=%d", sensor_val);  
#endif
```

A) 输出日志

B) 无输出

C) 编译错误

D) 运行时错误

**答案**：B（`DEBUG=0`导致预处理器删除该代码段）

------

**题目10**：在RTOS移植中，`__attribute__((section(".text.fast")))`的作用是？

A) 指定函数到高速内存区域

B) 强制函数内联

C) 标记中断处理函数

D) 优化编译速度

**答案**：A（针对TCM/ITCM等零等待状态存储器）

### 4. 存储类别

**题目1**：下列代码的输出是？

```c
#include <stdio.h>
void foo() {
    static int count = 0;
    count++;
    printf("%d ", count);
}
int main() { 
    for(int i=0; i<3; i++) foo();
    return 0;
}
```

A) 1 1 1

B) 1 2 3

C) 0 1 2

D) 编译错误

**答案**：B（static变量只初始化一次，保持状态）

------

**题目2**：在中断服务函数中应如何声明共享变量？

A) `int data;`

B) `static int data;`

C) `volatile int data;`

D) `const int data;`

**答案**：C（防止编译器优化导致读取过时值）

------

**题目3**：关于`extern`，错误的是？

A) 声明但不定义变量

B) 可在多文件间共享全局变量

C) `extern int val = 5;`是合法定义

D) 声明函数可省略extern

**答案**：C（带初始化是定义，导致链接多重定义错误）

------

**题目4**：代码块内`static int x;`的存储位置是？

A) 栈(stack)

B) 堆(heap)

C) 全局/静态区

D) 寄存器

**答案**：C（与全局变量同区，局部唯一性）

------

**题目5**：下列寄存器操作宏缺少哪个关键字？

```c
#define GPIOB_SET (* (uint32_t *)0x40020414) = 0x01  
void isr() {
    GPIOB_SET;  // 设置PB0
}
```

A) static

B) volatile

C) extern

D) const

**答案**：B（必须加volatile防止编译器优化掉“无用”写操作）

**正确写法**：`#define GPIOB_SET (* (volatile uint32_t *)0x40020414) = 0x01`

------

**题目6**：下列代码的输出？

```c
int global = 10;
void test() {
    int global = 20;
    printf("%d ", global);
}
int main() {
    test();
    printf("%d", global);
    return 0;
}
```

A) 20 10

B) 10 10

C) 20 20

D) 编译错误

**答案**：A（局部变量屏蔽全局变量）

------

**题目7**：`register`关键字在ARM Cortex-M中的实际作用是？

A) 强制变量存储于寄存器

B) 暗示编译器优先用寄存器

C) 提升内存访问速度

D) 保证原子操作

**答案**：B（C99后仅为优化提示，编译器**可忽略**）

------

**题目8**：多文件项目中，**正确定义全局变量**的方式：

A) `file1.c`: `int g_val;`

```
file2.c`: `extern int g_val;
```

B) `file1.c`: `static int g_val;`

```
file2.c`: `extern int g_val;
```

C) `file1.c`: `int g_val = 0;`

```
file2.c`: `int g_val = 0;
```

D) `file1.h`: `int g_val;`

**答案**：A（在.c定义，其他文件用extern声明）

------

**题目9**：RTOS任务间共享缓冲区应添加：

A) static

B) volatile + 互斥锁

C) extern

D) const

**答案**：B（volatile防优化，互斥锁防竞态）

------

**题目10**：下列代码行为是？

```c
void func(const int *p) {
    *p = 10; 
}
```

A) 修改常量值

B) 编译错误

C) 运行时错误

D) 修改成功

**答案**：B（const修饰的指针目标不可修改）

### 5. 类型转换

**题目1**：在32位系统中执行以下代码，输出结果是什么？

```c
uint16_t a = 500;
uint8_t b = 200;
uint32_t c = a * b;
printf("%lu", c);
```

A) 100000

B) 4464

C) 34464

D) 0

**答案**：A（100000）

**解析**：`a * b`执行前发生**整型提升** → `uint16_t * uint8_t`提升为 `int`（32位系统）

------

**题目2**：下列哪种转换可能导致精度损失？

A) double → float

B) int32_t → float

C) uint16_t → int32_t

D) int8_t → uint8_t

**答案**：A（double范围精度均大于float）

------

**题目3**：在STM32 HAL库中，GPIO引脚配置时，将引脚号转换为位掩码的正确方式是？

A) `(1 << pin)`

B) `(uint32_t)(pin)`

C) `*(volatile uint32_t*)pin`

D) `gpio_pin[pin]`

**答案**：A（如`GPIO_PIN_5`对应`0x0020`= `1<<5`）

------

**题目4**：执行以下代码后，值分别是？

```c
float f = 3.14f;
int i = f;        // (1)
int j = (int)f;   // (2)
```

A) (1)编译错误 (2)3

B) (1)3 (2)3

C) (1)未定义 (2)3

D) 都是3

**答案**：B（C语言允许float→int隐式截断）

------

**题目5**：关于指针转换，安全的是？

A) `int* p = malloc(sizeof(float)); *p = 5;`

B) `float f = 1.0; int i = *(int*)&f;`

C) `uint32_t addr = 0x40020000; GPIO_TypeDef* reg = (GPIO_TypeDef*)addr;`

D) B和C

**答案**：D

**解析**：B是**类型双关**标准做法（获取float的二进制表示）；C是嵌入式寄存器映射标准做法

------

**题目6**：执行下列代码输出？

```c
char c = -5;
unsigned int u = c;
printf("%u", u);
```

A) 251

B) 4294967291

C) 5

D) 编译警告

**答案**：B（0xFFFFFFFB）

------

**题目7**：在ARM Cortex-M中断处理中，将void*强制转换为函数指针：

```c
void (*handler)(void) = (void (*)(void))user_function;
```

该操作的风险是？

A) 内存泄漏

B) 对齐问题

C) 指令集不匹配（Thumb/ARM）

D) 栈溢出

**答案**：C（Cortex-M强制使用Thumb指令，若函数非Thumb编译则跳转失败）

------

**题目8**：下列浮点数转换代码存在什么问题？

```
double g = 1e200;
float f = g;
if (f == g) { /*...*/ }
```

A) 精度损失

B) 未定义行为

C) 比较总是true

D) 正确但效率低

**答案**：A（1e200超float范围→Inf，导致比较失败）

------

**题目9**：将0x12345678存储到小端系统的uint8_t数组中：

```
uint32_t word = 0x12345678;
uint8_t bytes[4];
memcpy(bytes, &word, 4);
```

bytes[0]的值是？

A) 0x12

B) 0x78

C) 0x34

D) 0x56

**答案**：B（小端系统低地址存低字节）

------

**题目10**：下列代码执行后`res`的值？

```
uint8_t x = 200;
uint8_t y = 100;
uint32_t res = x * y / 2;
```

A) 10000

B) 4464

C) 9996

D) 20000

**答案**：A（200 * 100=20,000 → 20,000/2=10,000）

------

**题目11**：下列指针转换哪项符合C++标准？

A) `const int* → void*`

B) `int* → const int*`

C) `int** → const int**`

D) `函数指针 → void*`

**答案**：B（添加const限定安全）

## 二、指针与内存管理

### 1. 指针运算

### 2. 动态内存

### 3. 智能指针

### 4. 内存布局

## 三、RTOS

### 1. 任务调度

### 2. 同步机制

### 3. 中断管理

### 4. 资源管理

## 四、Linux

### 1. 内核模块

### 2. 系统调用

### 3. 设备树

### 4. 文件系统

## 五、硬件交互与优化

### 1. 寄存器操作

### 2. 通信协议

### 3. 低功耗优化