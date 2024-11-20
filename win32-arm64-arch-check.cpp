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
    { "CRYPTO", PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE },
    { "CRC32", PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE },
    { "ATOMIC(v8.1)", PF_ARM_V81_ATOMIC_INSTRUCTIONS_AVAILABLE },
    { "DP(v8.2)", PF_ARM_V82_DP_INSTRUCTIONS_AVAILABLE       },
    { "JSCVT(v8.3)", PF_ARM_V83_JSCVT_INSTRUCTIONS_AVAILABLE    },
    { "LRCPC(v8.3)", PF_ARM_V83_LRCPC_INSTRUCTIONS_AVAILABLE    },
    { "SVE", PF_ARM_SVE_INSTRUCTIONS_AVAILABLE          },
    { "SVE2", PF_ARM_SVE2_INSTRUCTIONS_AVAILABLE         },
    { "SVE2.1", PF_ARM_SVE2_1_INSTRUCTIONS_AVAILABLE       },
    { "SVE-AES", PF_ARM_SVE_AES_INSTRUCTIONS_AVAILABLE      },
    { "SVE-PMULL128", PF_ARM_SVE_PMULL128_INSTRUCTIONS_AVAILABLE },
    { "SVE-BITPERM", PF_ARM_SVE_BITPERM_INSTRUCTIONS_AVAILABLE  },
    { "SVE-BF16", PF_ARM_SVE_BF16_INSTRUCTIONS_AVAILABLE     },
    { "SVE-EBF16", PF_ARM_SVE_EBF16_INSTRUCTIONS_AVAILABLE    },
    { "SVE-B16B16", PF_ARM_SVE_B16B16_INSTRUCTIONS_AVAILABLE   },
    { "SVE-SHA3", PF_ARM_SVE_SHA3_INSTRUCTIONS_AVAILABLE     },
    { "SVE-SM4", PF_ARM_SVE_SM4_INSTRUCTIONS_AVAILABLE      },
    { "SVE-I8MM", PF_ARM_SVE_I8MM_INSTRUCTIONS_AVAILABLE     },
    { "SVE-F32MM", PF_ARM_SVE_F32MM_INSTRUCTIONS_AVAILABLE    },
    { "SVE-F64MM", PF_ARM_SVE_F64MM_INSTRUCTIONS_AVAILABLE    },
};

const char * FeatureName (AArch64::Feature feature) noexcept;

int main () {
    SetLastError (0);
    if (AArch64::Initialize (/*SnapshotSnapdragon8cxGen3*/ /*SnapshotApple*/)) {

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
                                    std::printf (" %s", FeatureName (feature));
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
                                    std::printf (" %s", FeatureName (feature));
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

// FEAT_TO_UINT
//  - can we make this constexpr function that can be used in switch case below?
//
#define FEAT_TO_UINT(f) ((f.reg << 0) | (f.offset << 16) | (f.minimum << 24))

const char * FeatureName (AArch64::Feature feature) noexcept {
    using namespace AArch64;
    switch (feature.raw) {
        case FEAT_TO_UINT (Features::AES): return "AES";
        case FEAT_TO_UINT (Features::PMULL): return "PMULL";
        case FEAT_TO_UINT (Features::SHA1): return "SHA1";
        case FEAT_TO_UINT (Features::SHA256): return "SHA256";
        case FEAT_TO_UINT (Features::SHA512): return "SHA512";
        case FEAT_TO_UINT (Features::CRC32): return "CRC32";
        case FEAT_TO_UINT (Features::TME): return "TME";
        case FEAT_TO_UINT (Features::SHA3): return "SHA3";
        case FEAT_TO_UINT (Features::SM3): return "SM3";
        case FEAT_TO_UINT (Features::SM4): return "SM4";
        case FEAT_TO_UINT (Features::DotProd): return "DotProd";
        case FEAT_TO_UINT (Features::FHM): return "FHM";
        case FEAT_TO_UINT (Features::FlagM): return "FlagM";
        case FEAT_TO_UINT (Features::FlagM2): return "FlagM2";
        case FEAT_TO_UINT (Features::TLBIOS): return "TLBIOS";
        case FEAT_TO_UINT (Features::TLBIRANGE): return "TLBIRANGE";
        case FEAT_TO_UINT (Features::RNG): return "RNG";

        case FEAT_TO_UINT (Features::FP16): return "FP16";
        case FEAT_TO_UINT (Features::SVE): return "SVE";
        case FEAT_TO_UINT (Features::SEL2): return "SEL2";
        case FEAT_TO_UINT (Features::AMUv1): return "AMUv1";
        case FEAT_TO_UINT (Features::AMUv1p1): return "AMUv1.1";
        case FEAT_TO_UINT (Features::DIT): return "DIT";
        case FEAT_TO_UINT (Features::CSV2): return "CSV2";
        case FEAT_TO_UINT (Features::CSV2_2): return "CSV2.2";
        case FEAT_TO_UINT (Features::CSV2_3): return "CSV2.3";
        case FEAT_TO_UINT (Features::CSV3): return "CSV3";

        case FEAT_TO_UINT (Features::TTCNP): return "TTCNP";
        case FEAT_TO_UINT (Features::UAO): return "UAO";
        case FEAT_TO_UINT (Features::LSMAOC): return "LSMAOC";
        case FEAT_TO_UINT (Features::IESB): return "IESB";
        case FEAT_TO_UINT (Features::LVA): return "LVA";
        case FEAT_TO_UINT (Features::LVA3): return "LVA3";
        case FEAT_TO_UINT (Features::CCIDX): return "CCIDX";
        case FEAT_TO_UINT (Features::NV): return "NV";
        case FEAT_TO_UINT (Features::NV2): return "NV2";
        case FEAT_TO_UINT (Features::TTST): return "TTST";
        case FEAT_TO_UINT (Features::LSE2): return "LSE2";
        case FEAT_TO_UINT (Features::IDST): return "IDST";
        case FEAT_TO_UINT (Features::IDTE3): return "IDTE3";
        case FEAT_TO_UINT (Features::S2FWB): return "S2FWB";
        case FEAT_TO_UINT (Features::TTL): return "TTL";
        case FEAT_TO_UINT (Features::BBM): return "BBM";
        case FEAT_TO_UINT (Features::BBM_L2): return "BBM_L2";
        case FEAT_TO_UINT (Features::E0PD): return "E0PD";

        case FEAT_TO_UINT (Features::LSE): return "LSE";
        case FEAT_TO_UINT (Features::LSE128): return "LSE128";
        case FEAT_TO_UINT (Features::VHE): return "VHE";
        case FEAT_TO_UINT (Features::HPDS): return "HPDS";
        case FEAT_TO_UINT (Features::LOR): return "LOR";
        case FEAT_TO_UINT (Features::PAN): return "PAN";

        case FEAT_TO_UINT (Features::RDM): return "RDM";
        case FEAT_TO_UINT (Features::DPB): return "DPB";
        case FEAT_TO_UINT (Features::DPB2): return "DPB2";
        case FEAT_TO_UINT (Features::PAuth): return "PAuth";
        case FEAT_TO_UINT (Features::JSCVT): return "JSCVT";
        case FEAT_TO_UINT (Features::FCMA): return "FCMA";
        case FEAT_TO_UINT (Features::PAN2): return "PAN2";
        //case FEAT_TO_UINT (Features::UAO): return "UAO";
        
        case FEAT_TO_UINT (Features::RAS): return "RAS";
        case FEAT_TO_UINT (Features::RASv1p1): return "RASv1.1";
        case FEAT_TO_UINT (Features::RASv2): return "RASv2";

        //case FEAT_TO_UINT (Features::TTCNP): return "TTCNP";
        case FEAT_TO_UINT (Features::XNX): return "XNX";
        
        case FEAT_TO_UINT (Features::Debugv8p1): return "Debug8.1";
        case FEAT_TO_UINT (Features::Debugv8p2): return "Debug8.2";
        case FEAT_TO_UINT (Features::Debugv8p4): return "Debug8.4";
        case FEAT_TO_UINT (Features::Debugv8p8): return "Debug8.8";
        case FEAT_TO_UINT (Features::Debugv8p9): return "Debug8.9";

        case FEAT_TO_UINT (Features::LRCPC): return "LRCPC";
        case FEAT_TO_UINT (Features::LRCPC2): return "LRCPC2";
        case FEAT_TO_UINT (Features::LRCPC3): return "LRCPC3";

        case FEAT_TO_UINT (Features::BTI): return "BTI";
        case FEAT_TO_UINT (Features::SSBS): return "SSBS";
        case FEAT_TO_UINT (Features::SSBS2): return "SSBS2";
        case FEAT_TO_UINT (Features::MTE): return "MTE";
        case FEAT_TO_UINT (Features::MTE2): return "MTE2";
        case FEAT_TO_UINT (Features::MTE3): return "MTE3";
        case FEAT_TO_UINT (Features::SME): return "SME";
        case FEAT_TO_UINT (Features::SME2): return "SME2";
        case FEAT_TO_UINT (Features::RNDS): return "RNDS";
        case FEAT_TO_UINT (Features::NMI): return "NMI";
        case FEAT_TO_UINT (Features::GCS): return "GCS";
        case FEAT_TO_UINT (Features::THE): return "THE";
        case FEAT_TO_UINT (Features::DoubleFault2): return "DoubleFault2";
        case FEAT_TO_UINT (Features::PFAR): return "PFAR";
    }

    //return nullptr;

    static char debug [64];
    snprintf (debug, 64, "[%04X:%u >= %u]", feature.reg, feature.offset, feature.minimum);
    return debug;
}

#undef FEAT_TO_UINT
