#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>

MODULE_LICENSE("GPL");

typedef struct hci_dev *(*hci_dev_get_t)(int index);
typedef void (*hci_dev_put_t)(struct hci_dev *hdev);

static unsigned long lookup_name(const char *name) {
    struct kprobe kp = { .symbol_name = name };
    unsigned long addr;
    if (register_kprobe(&kp) < 0) return 0;
    addr = (unsigned long)kp.addr;
    unregister_kprobe(&kp);
    return addr;
}

static int __init fix_init(void) {
    hci_dev_get_t fn_hci_dev_get = (hci_dev_get_t)lookup_name("hci_dev_get");
    hci_dev_put_t fn_hci_dev_put = (hci_dev_put_t)lookup_name("hci_dev_put");

    if (fn_hci_dev_get && fn_hci_dev_put) {
        struct hci_dev *hdev = fn_hci_dev_get(0);
        if (hdev) {
            hdev->min_enc_key_size = 0;
            fn_hci_dev_put(hdev);
            pr_info("DualSense Fix: min_enc_key_size for hci0 set to 0\n");
        } else {
            pr_err("DualSense Fix: hci0 not found\n");
        }
    } else {
        pr_err("DualSense Fix: Could not resolve bluetooth symbols\n");
    }
    return 0;
}

static void __exit fix_exit(void) {}

module_init(fix_init);
module_exit(fix_exit);
