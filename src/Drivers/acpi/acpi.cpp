#include "acpi.h"
#include <asm-generic/errno-base.h>
#include "kernel/log.h"
#include "uacpi/uacpi.h"

i32 drivers::acpi::init() {
    uacpi_status ret = uacpi_initialize(0);
    if (uacpi_unlikely_error(ret)) {
        log::error("[ uACPI ] Failed to initialize uACPI: %s", uacpi_status_to_string(ret));
        return -ENODEV;
    }

    ret = uacpi_namespace_load();
    if (uacpi_unlikely_error(ret)) {
        log::error("[ uACPI ] Failed to load namespaces: %s", uacpi_status_to_string(ret));
        return -ENODEV;
    }

    ret = uacpi_namespace_initialize();
    if (uacpi_unlikely_error(ret)) {
        log::error("[ uACPI ] Failed to initialize namespaces: %s", uacpi_status_to_string(ret));
        return -ENODEV;
    }

    log::success("[ uACPI ] Initialized uACPI!");
    return 0;
}
