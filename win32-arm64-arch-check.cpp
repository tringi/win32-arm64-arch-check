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

struct PF {
    const char * name;
    DWORD        code;
} pfs [] = {
    { "CRYPTO",         PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE },
    { "CRC32",          PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE },
    { "ATOMIC(v8.1)",   PF_ARM_V81_ATOMIC_INSTRUCTIONS_AVAILABLE },
    { "DP(v8.2)",       PF_ARM_V82_DP_INSTRUCTIONS_AVAILABLE },
    { "JSCVT(v8.3)",    PF_ARM_V83_JSCVT_INSTRUCTIONS_AVAILABLE },
    { "LRCPC(v8.3)",    PF_ARM_V83_LRCPC_INSTRUCTIONS_AVAILABLE },
    { "SVE",            PF_ARM_SVE_INSTRUCTIONS_AVAILABLE },
    { "SVE2",           PF_ARM_SVE2_INSTRUCTIONS_AVAILABLE },
    { "SVE2.1",         PF_ARM_SVE2_1_INSTRUCTIONS_AVAILABLE },
    { "SVE-AES",        PF_ARM_SVE_AES_INSTRUCTIONS_AVAILABLE },
    { "SVE-PMULL128",   PF_ARM_SVE_PMULL128_INSTRUCTIONS_AVAILABLE },
    { "SVE-BITPERM",    PF_ARM_SVE_BITPERM_INSTRUCTIONS_AVAILABLE },
    { "SVE-BF16",       PF_ARM_SVE_BF16_INSTRUCTIONS_AVAILABLE },
    { "SVE-EBF16",      PF_ARM_SVE_EBF16_INSTRUCTIONS_AVAILABLE },
    { "SVE-B16B16",     PF_ARM_SVE_B16B16_INSTRUCTIONS_AVAILABLE},
    { "SVE-SHA3",       PF_ARM_SVE_SHA3_INSTRUCTIONS_AVAILABLE },
    { "SVE-SM4",        PF_ARM_SVE_SM4_INSTRUCTIONS_AVAILABLE },
    { "SVE-I8MM",       PF_ARM_SVE_I8MM_INSTRUCTIONS_AVAILABLE },
    { "SVE-F32MM",      PF_ARM_SVE_F32MM_INSTRUCTIONS_AVAILABLE },
    { "SVE-F64MM",      PF_ARM_SVE_F64MM_INSTRUCTIONS_AVAILABLE },
};

void DisplayFeatureName (AArch64::Feature feature) noexcept {
    if (feature.name && feature.name [0]) {
        if (std::strchr (feature.name, '+')) {
            
            // for higher levels of the same feature, where higher level implies lower, the name contains all, separated by '+'
            // e.g.: PMULL implies AES, so 'PMULL.name' returns "AES+PMULL"
            
            // but let's use space here

            char temp [128];
            for (auto i = 0u; i != sizeof temp; ++i) {
                if (feature.name [i] == '+') {
                    temp [i] = ' ';
                } else {
                    temp [i] = feature.name [i];
                }
                if (feature.name [i] == '\0')
                    break;
            }

            std::printf (" %s", temp);
        } else {
            std::printf (" %s", feature.name);
        }
    } else {
        std::printf (" [%04X:%u >= %u]", feature.reg, feature.offset, feature.minimum);
    }
}

int main () {
    SetLastError (0);
    if (AArch64::Initialize (SnapshotApple)) { /*SnapshotSnapdragon8cxGen3*/ /*SnapshotApple*/

        std::printf ("OS Processor Feature report:\n  ");
        bool anypf = false;
        for (auto & [name, code] : pfs) {
            if (IsProcessorFeaturePresent (code)) {
                std::printf ("%s ", name);
                anypf = true;
            }
        }
        if (!anypf) {
            std::printf ("none");
        }
        std::printf ("\n\n");

        auto sets = AArch64::HeterogeneitySets ();
        std::printf ("%zu distinct ARM cores in %zu sets\n\n", AArch64::Heterogeneity (), sets.size ());

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
                                    DisplayFeatureName (feature);
                                    any = true;
                                } else {
                                    missing = true;
                                }
                            }
                        }
                    }
                    if (missing) {
                        if (any) {
                            std::printf ("\n        ");
                        } else {
                            std::printf (" all");
                        }
                        std::printf (" missing:");
                        for (std::size_t s = 0; s != (std::size_t) AArch64::Strictness::Count; ++s) {
                            for (const auto & feature : level.features [s]) {
                                if (!AArch64::Check (processor, feature)) {
                                    DisplayFeatureName (feature);
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
