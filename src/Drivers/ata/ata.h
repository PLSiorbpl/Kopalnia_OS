#pragma once

namespace drivers::ata {
    enum class drive_type {
        PATA_PI,
        SATA_PI,
        PATA,
        SATA,
        UNKNOWN,
    };

    enum class soft_reset_error {
        floating_bus,
        pata_pi_dev,
        timeout,
        success,
    };

    class device {
        public:
        explicit device(bool slave);
        [[nodiscard]] soft_reset_error soft_reset();

        private:
        static bool check_command(bool drq);

        drive_type type;
    };
}
