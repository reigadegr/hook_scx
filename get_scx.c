#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/kallsyms.h>

// 声明 scx_init 函数指针，但不在这里初始化
void (*scx_init)(void);

static int __init hook_module_init(void)
{
    // 在模块初始化函数中查找 scx_init 的地址
    scx_init = (void (*)(void))kallsyms_lookup_name("scx_init");

    if (scx_init) {
        // scx_init();
       printk(KERN_INFO "get_scx:: scx_init called successfully. Address: 0x%p\n", scx_init);
    } else {
        printk(KERN_ALERT "get_scx Failed to find scx_init symbol\n");
    }
    return 0;
}

late_initcall(hook_module_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A module to call scx_init directly");
