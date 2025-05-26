#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/kallsyms.h>
#include <linux/kprobes.h>
#include <linux/string.h>

#define OPLUS_MODULE_NAME "oplus_bsp_sched_ext"

static void (*scx_init)(void) = NULL;
static struct kretprobe init_module_kretprobe;

// 定义函数指针类型，避免与内核 find_module 冲突
typedef struct module *(*find_module_t)(const char *name);
static find_module_t real_find_module = NULL;


static bool need_hook = true;
// kretprobe 返回处理函数
static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    // 检查目标模块是否已加载
    if (real_find_module && real_find_module(OPLUS_MODULE_NAME) && scx_init) {
        pr_info("get_scx: Detected %s loaded, calling scx_init!\n", OPLUS_MODULE_NAME);
        if (need_hook){
            scx_init();
            // regs->regs[0] = 0; // ARM64: 设置返回值为 0
            // need_hook = false;
        }else{
			pr_info("get_scx: Do not hook");
		}
    }
    return 0;
}

static int __init get_scx_init(void)
{
    int ret;

    scx_init = (void (*)(void))kallsyms_lookup_name("scx_init");
    if (!scx_init) {
        pr_alert("get_scx: Failed to find scx_init symbol\n");
        return 0;
    }
    pr_info("get_scx: Found scx_init at %p\n", scx_init);

    real_find_module = (find_module_t)kallsyms_lookup_name("find_module");
    if (!real_find_module) {
        pr_alert("get_scx: Failed to find find_module symbol\n");
        return 0;
    }

    memset(&init_module_kretprobe, 0, sizeof(init_module_kretprobe));
    init_module_kretprobe.kp.symbol_name = "__arm64_sys_init_module";
    init_module_kretprobe.handler = ret_handler;
    init_module_kretprobe.maxactive = 1;

    ret = register_kretprobe(&init_module_kretprobe);
    if (ret < 0) {
        pr_alert("get_scx: register_kretprobe failed, returned %d\n", ret);
        return 0;
    }

    pr_info("get_scx: kretprobe registered for __arm64_sys_init_module\n");
    return 0;
}

static void __exit get_scx_exit(void)
{
    unregister_kretprobe(&init_module_kretprobe);
    pr_info("get_scx: kretprobe unregistered\n");
}

core_initcall(get_scx_init);
module_exit(get_scx_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A module to call scx_init when oplus_bsp_sched_ext loads");