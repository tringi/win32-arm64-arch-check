#ifndef AARCH64CHECK_H
#define AARCH64CHECK_H

#include <Windows.h>
#include <cstdint>
#include <vector>
#include <span>
#include <map>
#include <set>

namespace AArch64 {
    namespace Register {
        static constexpr WORD MIDR_EL1 = 0x4000;
        static constexpr WORD ID_AA64PFR0_EL1 = 0x4020;
        static constexpr WORD ID_AA64PFR1_EL1 = 0x4021;
        static constexpr WORD ID_AA64DFR0_EL1 = 0x4028;
        static constexpr WORD ID_AA64DFR1_EL1 = 0x4029;
        static constexpr WORD ID_AA64ISAR0_EL1 = 0x4030; // Instruction Set Attribute Register 0
        static constexpr WORD ID_AA64ISAR1_EL1 = 0x4031;
        static constexpr WORD ID_AA64ISAR2_EL1 = 0x4032;
        static constexpr WORD ID_AA64MMFR0_EL1 = 0x4038;
        static constexpr WORD ID_AA64MMFR1_EL1 = 0x4039;
        static constexpr WORD ID_AA64MMFR2_EL1 = 0x403A;
        static constexpr WORD ID_AA64MMFR3_EL1 = 0x403B;
        static constexpr WORD SCTLR_EL1 = 0x4080; // System Control Register
        static constexpr WORD CPACR_EL1 = 0x4081;
        static constexpr WORD TTBR0_EL1 = 0x4100; // Translation Table Base Register
        static constexpr WORD TTBR1_EL1 = 0x4101;
        static constexpr WORD MAIR_EL1 = 0x4510; // Memory Attribute Indirection Register
    }

    struct Feature {
        union {
            struct {
                WORD reg;
                BYTE offset;
                BYTE minimum;
            };
            UINT raw;
        };

        inline auto operator <=> (Feature other) const noexcept { return this->raw <=> other.raw; }
        inline auto operator == (Feature other) const noexcept { return this->raw == other.raw; }
    };

    namespace Features {
        static constexpr Feature Null = { 0,0,0 }; // identity feature to simplify spans below, always check true

        static constexpr Feature AES    = { Register::ID_AA64ISAR0_EL1, 4,  1 };
        static constexpr Feature PMULL  = { Register::ID_AA64ISAR0_EL1, 4,  2 }; // + AES
        static constexpr Feature SHA1   = { Register::ID_AA64ISAR0_EL1, 8,  1 };
        static constexpr Feature SHA256 = { Register::ID_AA64ISAR0_EL1, 12, 1 };
        static constexpr Feature SHA512 = { Register::ID_AA64ISAR0_EL1, 12, 2 }; // + 256
        static constexpr Feature CRC32  = { Register::ID_AA64ISAR0_EL1, 16, 1 };
        static constexpr Feature LSE    = { Register::ID_AA64ISAR0_EL1, 20, 2 }; // v8.1, Large Systems Extension, Atomic instructions LDADD, LDCLR, LDEOR, LDSET, LDSMAX, LDSMIN, LDUMAX, LDUMIN, CAS, CASP, and SWP
        static constexpr Feature LSE128 = { Register::ID_AA64ISAR0_EL1, 20, 3 }; // opt., Large Systems Extension, FEAT_LSE plus 128-bit instructions LDCLRP, LDSETP, and SWPP
        static constexpr Feature TME    = { Register::ID_AA64ISAR0_EL1, 24, 1 };
        static constexpr Feature RDM    = { Register::ID_AA64ISAR0_EL1, 28, 1 }; // v8.2
        static constexpr Feature SHA3   = { Register::ID_AA64ISAR0_EL1, 32, 1 };
        static constexpr Feature SM3    = { Register::ID_AA64ISAR0_EL1, 36, 1 };
        static constexpr Feature SM4    = { Register::ID_AA64ISAR0_EL1, 40, 1 };
        static constexpr Feature DotProd= { Register::ID_AA64ISAR0_EL1, 44, 1 };
        static constexpr Feature FHM    = { Register::ID_AA64ISAR0_EL1, 48, 1 };
        static constexpr Feature FlagM  = { Register::ID_AA64ISAR0_EL1, 52, 1 };
        static constexpr Feature FlagM2 = { Register::ID_AA64ISAR0_EL1, 52, 2 };
        static constexpr Feature TLBIOS = { Register::ID_AA64ISAR0_EL1, 56, 1 };
        static constexpr Feature TLBIRANGE={Register::ID_AA64ISAR0_EL1, 56, 2 };
        static constexpr Feature RNG    = { Register::ID_AA64ISAR0_EL1, 60, 1 };

        static constexpr Feature DPB    = { Register::ID_AA64ISAR1_EL1, 0,  1 }; // v8.2
        static constexpr Feature DPB2   = { Register::ID_AA64ISAR1_EL1, 0,  2 }; // v8.2
        static constexpr Feature PAuth  = { Register::ID_AA64ISAR1_EL1, 4,  1 }; // v8.3
        static constexpr Feature EPAC   = { Register::ID_AA64ISAR1_EL1, 4,  2 };
        static constexpr Feature PAuth2 = { Register::ID_AA64ISAR1_EL1, 4,  3 };
        static constexpr Feature FPAC   = { Register::ID_AA64ISAR1_EL1, 4,  4 };
        static constexpr Feature FPACCOMBINE = { Register::ID_AA64ISAR1_EL1, 4, 5 };
        static constexpr Feature PAuth_LR={ Register::ID_AA64ISAR1_EL1, 4, 6 };
        static constexpr Feature JSCVT  = { Register::ID_AA64ISAR1_EL1, 12, 1 }; // v8.3
        static constexpr Feature FCMA   = { Register::ID_AA64ISAR1_EL1, 16, 1 }; // v8.3
        static constexpr Feature LRCPC  = { Register::ID_AA64ISAR1_EL1, 20, 1 }; // v8.3
        static constexpr Feature LRCPC2 = { Register::ID_AA64ISAR1_EL1, 20, 2 }; // v8.4
        static constexpr Feature LRCPC3 = { Register::ID_AA64ISAR1_EL1, 20, 3 };
        static constexpr Feature PACQARMA5={Register::ID_AA64ISAR1_EL1, 24, 1 };
        static constexpr Feature PACIMP = { Register::ID_AA64ISAR1_EL1, 28, 1 };
        static constexpr Feature FRINTTS= { Register::ID_AA64ISAR1_EL1, 32, 1 };
        static constexpr Feature SB     = { Register::ID_AA64ISAR1_EL1, 36, 1 };
        static constexpr Feature SPECRES= { Register::ID_AA64ISAR1_EL1, 40, 1 };
        static constexpr Feature SPECRES2={ Register::ID_AA64ISAR1_EL1, 40, 2 };
        static constexpr Feature BF16   = { Register::ID_AA64ISAR1_EL1, 44, 1 };
        static constexpr Feature EBF16  = { Register::ID_AA64ISAR1_EL1, 44, 2 };
        static constexpr Feature DGH    = { Register::ID_AA64ISAR1_EL1, 48, 1 };
        static constexpr Feature I8MM   = { Register::ID_AA64ISAR1_EL1, 52, 1 };
        static constexpr Feature XS     = { Register::ID_AA64ISAR1_EL1, 56, 1 };
        static constexpr Feature LS64   = { Register::ID_AA64ISAR1_EL1, 60, 1 };
        static constexpr Feature LS64_V = { Register::ID_AA64ISAR1_EL1, 60, 2 };
        static constexpr Feature LS64_ACCDATA = { Register::ID_AA64ISAR1_EL1, 60, 3 };
        static constexpr Feature LS64WB = { Register::ID_AA64ISAR1_EL1, 60, 4 };

        static constexpr Feature WFxT   = { Register::ID_AA64ISAR2_EL1, 0,  2 };
        static constexpr Feature MOPS   = { Register::ID_AA64ISAR2_EL1, 16, 1 };
        static constexpr Feature HBC    = { Register::ID_AA64ISAR2_EL1, 20, 1 };
        static constexpr Feature CLRBHB = { Register::ID_AA64ISAR2_EL1, 28, 1 };
        static constexpr Feature CSSC   = { Register::ID_AA64ISAR2_EL1, 52, 1 };
        static constexpr Feature CMPBR  = { Register::ID_AA64ISAR2_EL1, 52, 2 };

        static constexpr Feature FGT    = { Register::ID_AA64MMFR0_EL1, 56, 1 };
        static constexpr Feature FGT2   = { Register::ID_AA64MMFR0_EL1, 56, 2 };
        static constexpr Feature ECV    = { Register::ID_AA64MMFR0_EL1, 60, 1 };

        static constexpr Feature VHE    = { Register::ID_AA64MMFR1_EL1, 8,  1 }; // v8.1, but not reported inside of Hyper-V VMs
        static constexpr Feature HPDS   = { Register::ID_AA64MMFR1_EL1, 12, 1 }; // v8.1, but not reported by Snapdragon 8cx Gen3
        static constexpr Feature HPDS2  = { Register::ID_AA64MMFR1_EL1, 12, 2 };
        static constexpr Feature LOR    = { Register::ID_AA64MMFR1_EL1, 16, 1 }; // v8.1
        static constexpr Feature PAN    = { Register::ID_AA64MMFR1_EL1, 20, 1 }; // v8.1
        static constexpr Feature PAN2   = { Register::ID_AA64MMFR1_EL1, 20, 2 }; // v8.2
        static constexpr Feature PAN3   = { Register::ID_AA64MMFR1_EL1, 20, 3 };
        static constexpr Feature XNX    = { Register::ID_AA64MMFR1_EL1, 28, 1 }; // v8.2
        static constexpr Feature ETS2   = { Register::ID_AA64MMFR1_EL1, 36, 2 };
        static constexpr Feature ETS3   = { Register::ID_AA64MMFR1_EL1, 36, 3 };
        static constexpr Feature TIDCP1 = { Register::ID_AA64MMFR1_EL1, 52, 1 };
        static constexpr Feature CMOW   = { Register::ID_AA64MMFR1_EL1, 56, 1 };
        static constexpr Feature ECBHB  = { Register::ID_AA64MMFR1_EL1, 60, 1 };

        static constexpr Feature TTCNP  = { Register::ID_AA64MMFR2_EL1, 0,  1 }; // v8.2
        static constexpr Feature UAO    = { Register::ID_AA64MMFR2_EL1, 4,  1 }; // v8.2
        static constexpr Feature LSMAOC = { Register::ID_AA64MMFR2_EL1, 8,  1 };
        static constexpr Feature IESB   = { Register::ID_AA64MMFR2_EL1, 12, 1 };
        static constexpr Feature LVA    = { Register::ID_AA64MMFR2_EL1, 16, 1 };
        static constexpr Feature LVA3   = { Register::ID_AA64MMFR2_EL1, 16, 2 };
        static constexpr Feature CCIDX  = { Register::ID_AA64MMFR2_EL1, 20, 1 };
        static constexpr Feature NV     = { Register::ID_AA64MMFR2_EL1, 24, 1 };
        static constexpr Feature NV2    = { Register::ID_AA64MMFR2_EL1, 24, 2 };
        static constexpr Feature TTST   = { Register::ID_AA64MMFR2_EL1, 28, 1 };
        static constexpr Feature LSE2   = { Register::ID_AA64MMFR2_EL1, 32, 1 };
        static constexpr Feature IDST   = { Register::ID_AA64MMFR2_EL1, 36, 1 };
        static constexpr Feature IDTE3  = { Register::ID_AA64MMFR2_EL1, 36, 2 };
        static constexpr Feature S2FWB  = { Register::ID_AA64MMFR2_EL1, 40, 1 };
        static constexpr Feature TTL    = { Register::ID_AA64MMFR2_EL1, 48, 1 };
        static constexpr Feature BBM    = { Register::ID_AA64MMFR2_EL1, 52, 1 };
        static constexpr Feature BBM_L2 = { Register::ID_AA64MMFR2_EL1, 52, 2 };
        static constexpr Feature E0PD   = { Register::ID_AA64MMFR2_EL1, 60, 1 };

        static constexpr Feature TCR2   = { Register::ID_AA64MMFR3_EL1, 0,  1 }; // v8.9
        static constexpr Feature SCTLR2 = { Register::ID_AA64MMFR3_EL1, 4,  1 }; // v8.9

        static constexpr Feature FP16   = { Register::ID_AA64PFR0_EL1, 20, 1 }; 
        static constexpr Feature RAS    = { Register::ID_AA64PFR0_EL1, 28, 1 }; // v8.2
        static constexpr Feature RASv1p1= { Register::ID_AA64PFR0_EL1, 28, 2 };
        static constexpr Feature RASv2  = { Register::ID_AA64PFR0_EL1, 28, 3 };
        static constexpr Feature SVE    = { Register::ID_AA64PFR0_EL1, 32, 1 }; // ID_AA64ZFR0_EL1
        static constexpr Feature SEL2   = { Register::ID_AA64PFR0_EL1, 36, 1 };
        static constexpr Feature AMUv1  = { Register::ID_AA64PFR0_EL1, 44, 1 };
        static constexpr Feature AMUv1p1= { Register::ID_AA64PFR0_EL1, 44, 2 };
        static constexpr Feature DIT    = { Register::ID_AA64PFR0_EL1, 48, 1 };
        static constexpr Feature CSV2   = { Register::ID_AA64PFR0_EL1, 56, 1 };
        static constexpr Feature CSV2_2 = { Register::ID_AA64PFR0_EL1, 56, 2 };
        static constexpr Feature CSV2_3 = { Register::ID_AA64PFR0_EL1, 56, 3 };
        static constexpr Feature CSV3   = { Register::ID_AA64PFR0_EL1, 60, 1 };

        static constexpr Feature BTI    = { Register::ID_AA64PFR1_EL1, 0, 1 };
        static constexpr Feature SSBS   = { Register::ID_AA64PFR1_EL1, 4, 1 };
        static constexpr Feature SSBS2  = { Register::ID_AA64PFR1_EL1, 4, 2 };
        static constexpr Feature MTE    = { Register::ID_AA64PFR1_EL1, 8, 1 };
        static constexpr Feature MTE2   = { Register::ID_AA64PFR1_EL1, 8, 2 };
        static constexpr Feature MTE3   = { Register::ID_AA64PFR1_EL1, 8, 3 };
        static constexpr Feature SME    = { Register::ID_AA64PFR1_EL1, 24, 1 };
        static constexpr Feature SME2   = { Register::ID_AA64PFR1_EL1, 24, 2 };
        static constexpr Feature RNDS   = { Register::ID_AA64PFR1_EL1, 28, 1 };
        static constexpr Feature NMI    = { Register::ID_AA64PFR1_EL1, 36, 1 };
        static constexpr Feature GCS    = { Register::ID_AA64PFR1_EL1, 44, 1 }; // Guarded Control Stack
        static constexpr Feature THE    = { Register::ID_AA64PFR1_EL1, 48, 1 }; // Translation Hardening Extension
        static constexpr Feature DoubleFault2 = { Register::ID_AA64PFR1_EL1, 56, 1 };
        static constexpr Feature PFAR   = { Register::ID_AA64PFR1_EL1, 60, 1 };


        static constexpr Feature Debugv8p1 = { Register::ID_AA64DFR0_EL1, 0, 0b0111 };
        static constexpr Feature Debugv8p2 = { Register::ID_AA64DFR0_EL1, 0, 0b1000 }; // v8.2
        static constexpr Feature Debugv8p4 = { Register::ID_AA64DFR0_EL1, 0, 0b1001 };
        static constexpr Feature Debugv8p8 = { Register::ID_AA64DFR0_EL1, 0, 0b1010 };
        static constexpr Feature Debugv8p9 = { Register::ID_AA64DFR0_EL1, 0, 0b1011 };

    }
    namespace Sets {
        // TODO: Completely re-calibrate (only up to v8.3 is somewhat calibrated):
        //   Minimal - instructions generated by MSVC for that given /arch:ARMvX.Y for user-mode code
        //   Relaxed - extensions otherwise present on all (or majority) devices declared on this level
        //   Strict - extensions mandatory by ARM specification

        static constexpr Feature v8_1_Minimal [] = { Features::LSE, Features::PAN };
        static constexpr Feature v8_1_Relaxed [] = { Features::Null };
        static constexpr Feature v8_1_Strict  [] = { Features::HPDS, Features::LOR, Features::VHE };

        static constexpr Feature v8_2_Minimal [] = { Features::RDM, Features::DPB, Features::PAN2, Features::UAO };
        static constexpr Feature v8_2_Relaxed [] = { Features::RAS };
        static constexpr Feature v8_2_Strict  [] = { Features::TTCNP, Features::XNX, Features::Debugv8p2 }; // how to check FEAT_ASMv8p2 ?

        static constexpr Feature v8_3_Minimal [] = { Features::LRCPC, Features::PAuth };
        static constexpr Feature v8_3_Relaxed [] = { Features::Null }; // Features::FCMA ?
        static constexpr Feature v8_3_Strict  [] = { Features::Null }; // Features::JSCVT ?

        static constexpr Feature v8_4_Minimal [] = { Features::DIT, Features::LRCPC2, Features::FlagM };
        static constexpr Feature v8_4_Relaxed [] = { Features::Debugv8p4 };
        static constexpr Feature v8_4_Strict  [] = { Features::IDST, Features::LSE2, Features::TLBIOS, Features::TLBIRANGE, Features::S2FWB, Features::BBM, Features::TTL };

        static constexpr Feature v8_5_Minimal [] = { Features::CSV2, Features::CSV3, Features::BTI, Features::FlagM2, Features::DPB2, Features::FRINTTS, Features::SB, Features::SPECRES, Features::E0PD };
        static constexpr Feature v8_5_Relaxed [] = { Features::Null };
        static constexpr Feature v8_5_Strict  [] = { Features::Null };

        static constexpr Feature v8_6_Minimal [] = { Features::BF16, Features::FGT, Features::ECV };
        static constexpr Feature v8_6_Relaxed [] = { Features::I8MM  };
        static constexpr Feature v8_6_Strict  [] = { Features::PAuth2 };

        static constexpr Feature v8_7_Minimal [] = { Features::XS, Features::WFxT, Features::PAN3 };
        static constexpr Feature v8_7_Relaxed [] = { Features::Null };
        static constexpr Feature v8_7_Strict  [] = { Features::Null };

        static constexpr Feature v8_8_Minimal [] = { Features::NMI, Features::Debugv8p8, Features::MOPS, Features::HBC, Features::ETS2, Features::CMOW, Features::TIDCP1  };
        static constexpr Feature v8_8_Relaxed [] = { Features::Null };
        static constexpr Feature v8_8_Strict  [] = { Features::Null };

        static constexpr Feature v8_9_Minimal [] = { Features::SPECRES2, Features::CLRBHB, Features::CSSC, Features::ECBHB, Features::FGT2 };
        static constexpr Feature v8_9_Relaxed [] = { Features::Debugv8p9 };
        static constexpr Feature v8_9_Strict  [] = { Features::SCTLR2, Features::TCR2 };

        static constexpr Feature v9_0_Minimal [] = { Features::DotProd, Features::SVE, Features::FHM };
        static constexpr Feature v9_0_Relaxed [] = { Features::FP16, Features::RAS };
        static constexpr Feature v9_0_Strict  [] = { Features::FRINTTS };

        static constexpr Feature v9_1_Minimal [] = { Features::Null };
        static constexpr Feature v9_1_Relaxed [] = { Features::Null };
        static constexpr Feature v9_1_Strict  [] = { Features::BF16 };

        /*static constexpr Feature v9_4_Minimal [] = { Features::CSSC };
        static constexpr Feature v9_4_Relaxed [] = { Features::Null };
        static constexpr Feature v9_4_Strict  [] = { Features::CHK };

        static constexpr Feature v9_5_Minimal [] = { Features::ETS3 };
        static constexpr Feature v9_5_Relaxed [] = { Features::Null };
        static constexpr Feature v9_5_Strict  [] = { Features::ASID2, Features::CPA, Features::STEP2 };

        static constexpr Feature v9_6_Minimal [] = { Features::CMPBR };
        static constexpr Feature v9_6_Relaxed [] = { Features::Null };
        static constexpr Feature v9_6_Strict  [] = { Features::FPRCVT, Features::LSUI, Features::OCCMO, Features::SRMASK, Features::UINJ };
        */

        //static constexpr Feature Optional [] = { Features::... };
    }

    // Strictness
    //  - feature check strictness
    //  - TODO: nomenclature?
    //
    enum class Strictness {
        Minimal, // matches only minimal set of features, certain to be found
        Relaxed, // matches feature values generally found on the architecture of that level as claimed by manufactures
        Strict,  // strictly match register values to ARMvX.Y mandatory requirements
        Count,
    };

    struct Level {
        WORD name; // e.g. 0x08'01 for ARMv8.1
        WORD baseline;
        std::span <const Feature> features [(std::size_t) Strictness::Count];
    };

    static constexpr Level Levels [] = {
        { 0x801, 0x000, { Sets::v8_1_Minimal, Sets::v8_1_Relaxed, Sets::v8_1_Strict } },
        { 0x802, 0x801, { Sets::v8_2_Minimal, Sets::v8_2_Relaxed, Sets::v8_2_Strict } },
        { 0x803, 0x802, { Sets::v8_3_Minimal, Sets::v8_3_Relaxed, Sets::v8_3_Strict } },
        { 0x804, 0x803, { Sets::v8_4_Minimal, Sets::v8_4_Relaxed, Sets::v8_4_Strict } },
        { 0x805, 0x804, { Sets::v8_5_Minimal, Sets::v8_5_Relaxed, Sets::v8_5_Strict } },
        { 0x806, 0x805, { Sets::v8_6_Minimal, Sets::v8_6_Relaxed, Sets::v8_6_Strict } },
        { 0x807, 0x806, { Sets::v8_7_Minimal, Sets::v8_7_Relaxed, Sets::v8_7_Strict } },
        { 0x808, 0x807, { Sets::v8_8_Minimal, Sets::v8_8_Relaxed, Sets::v8_8_Strict } },
        { 0x809, 0x808, { Sets::v8_9_Minimal, Sets::v8_9_Relaxed, Sets::v8_9_Strict } },
        { 0x900, 0x806, { Sets::v9_0_Minimal, Sets::v9_0_Relaxed, Sets::v9_0_Strict } },
        { 0x901, 0x900, { Sets::v9_1_Minimal, Sets::v9_1_Relaxed, Sets::v9_1_Strict } },
    };

    // Initialize
    //  - reads local device data and initializes working dataset
    //
    bool Initialize ();

    // Initialize (dataset)
    //  - provides alternate data for examination
    //
    bool Initialize (const std::vector <std::map <std::uint16_t, std::uint64_t>> & alternative_dataset);

    // Heterogeneity
    //  - determines number of distinct feature sets among available logical processors
    //
    std::size_t Heterogeneity ();

    // HeterogeneitySets
    //  - returns processor numbers that begin different feature set; the last entry being final count
    //  - e.g.: [6, 8] for Snapdragon 7c (6+2)
    //          [4, 8] for Snapdragon 835 (4+4)
    //  - number of entries may be larger than the value 'Heterogeneity()' returns
    //
    std::vector <UINT> HeterogeneitySets ();

    // ProcessorNumberToIndex
    //  - helper
    //
    inline UINT ProcessorNumberToIndex (PROCESSOR_NUMBER p) noexcept {
        return p.Group * 64 + p.Number;
    }

    // Check
    //  - checks the presence of a particular 'feature' on a 'processor' (0-based index)
    //
    bool Check (UINT processor, Feature) noexcept;

    // Determine
    //  - 
    //  - returns: 0 - on failure (not ARM64, missing data, ...), call GetLastError ()
    //             0x800 - for ARMv8.0, a baseline, Windows on ARM requires v8.0 + CRC32, AES and SHA1
    //             0x801 - for ARMv8.1
    //             ...
    //             0x809 - for ARMv8.9
    //             0x900 - for ARMv9.0
    //             ...
    //             0x905 - for ARMv9.5
    //
    WORD Determine (UINT processor, Strictness = Strictness::Relaxed) noexcept;
}

#endif
