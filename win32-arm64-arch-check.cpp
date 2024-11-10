#include <Windows.h>
#include <cstdio>
#include <vector>

#include "AArch64check.h"

std::vector <std::map <std::uint16_t, std::uint64_t>> SnapshotApple = {
    {
       { 0x4020, 0x1101000010111111 },
       { 0x4021, 0x20 },
       { 0x4028, 0x10305006 },
       { 0x4030, 0x221100110212120 },
       { 0x4031, 0x11110211202 },
       { 0x4038, 0x10000f100001 },
       { 0x4039, 0x11212000 },
       { 0x403A, 0x1001001100001011 },
       { 0x4080, 0x100030d059dd },
       { 0x4100, 0x800001b75c0000 },
       { 0x4510, 0x444400ff444400ff },
       { 0x5801, 0x8444c004 },
    }
};
std::vector <std::map <std::uint16_t, std::uint64_t>> SnapshotSnapdragon8cxGen3 = {
    {
       { 0x4020, 1224979098931106066 },
       { 0x4021, 16 },
       { 0x4028, 271602696 },
       { 0x4030, 4521192084017440 },
       { 0x4031, 18874417 },
       { 0x4038, 1048609 },
       { 0x4039, 2097154 },
       { 0x403A, 16 },
       { 0x4080, 17593005005149 },
       { 0x4100, 54043212695154688 },
       { 0x4510, 4919057789357392127 },
       { 0x5801, 2487533572 },
    }
};


int main () {
    SetLastError (0);
    if (AArch64::Initialize ()) {

        //AArch64::SetWorkingData (SnapshotSnapdragon8cxGen3);
        //AArch64::SetWorkingData (SnapshotApple);

        auto sets = AArch64::HeterogeneitySets ();
        std::printf ("%u distinct ARM cores in %zu sets\n\n", AArch64::Heterogeneity (), sets.size ());

        UINT first = 0;
        for (UINT cpu_set = 0u; cpu_set != sets.size (); ++cpu_set) {
            std::printf ("CPUs %u..%u ISA Level:\n", first, sets [cpu_set] - 1);

            auto processor = 0u;
            if (auto result = AArch64::Determine (processor, AArch64::Strictness::Strict)) {
                std::printf ("  Strict:  ARMv%u.%u\n", HIBYTE (result), LOBYTE (result));

                result = AArch64::Determine (processor, AArch64::Strictness::Relaxed);
                std::printf ("  Relaxed: ARMv%u.%u\n", HIBYTE (result), LOBYTE (result));

                result = AArch64::Determine (processor, AArch64::Strictness::Minimal);
                std::printf ("  Minimal: ARMv%u.%u\n", HIBYTE (result), LOBYTE (result));

                // features:

                std::printf ("\n  ISA Features:\n");

                for (const auto & level : AArch64::Levels) {
                    std::printf ("    %u.%u:", HIBYTE (level.name), LOBYTE (level.name));

                    bool any = false;
                    bool missing = false;
                    for (std::size_t s = 0; s != (std::size_t) AArch64::Strictness::Count; ++s) {
                        for (const auto & feature : level.features [s]) {
                            if (feature != AArch64::Features::Null) {
                                if (AArch64::Check (processor, feature)) {
                                    std::printf (" %s", AArch64::FeatureName (feature));
                                    any = true;
                                } else {
                                    missing = true;
                                }
                            }
                        }
                    }
                    if (missing) {
                        if (any) {
                            std::printf (",");
                        } else {
                            std::printf (" all");
                        }
                        std::printf (" missing:");
                        for (std::size_t s = 0; s != (std::size_t) AArch64::Strictness::Count; ++s) {
                            for (const auto & feature : level.features [s]) {
                                if (!AArch64::Check (processor, feature)) {
                                    std::printf (" %s", AArch64::FeatureName (feature));
                                }
                            }
                        }
                    }

                    std::printf ("\n");
                }
                std::printf ("\n");
            } else {
                std::printf ("  not ARM64 device or ERROR (%lu)\n", GetLastError ());
            }

            first = sets [cpu_set];
        }
    } else {
        std::printf ("AArch64::Initialize failed, ERROR (%lu)\n", GetLastError ());
    }
    return GetLastError ();
}
