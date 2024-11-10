#include "AArch64check.h"

namespace {
    std::vector <std::map <std::uint16_t, std::uint64_t>> registers; // <register, value> [processor]
}

bool AArch64::Initialize () {
    bool result = false;

    registers.clear ();
    registers.resize (GetActiveProcessorCount (ALL_PROCESSOR_GROUPS));
    for (UINT processor = 0; ; ++processor) {

        wchar_t szRegPath [64];
        std::swprintf (szRegPath, 64, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%u", processor);

        HKEY hKeyCPU = NULL;
        DWORD dwError;
        if ((dwError = RegOpenKeyEx (HKEY_LOCAL_MACHINE, szRegPath, 0, KEY_READ, &hKeyCPU)) == ERROR_SUCCESS) {

            if (registers.size () <= std::size_t (processor)) {
                registers.resize (std::size_t (processor) + 1);
            }

            DWORD index = 0;
            DWORD dwValueType;

            wchar_t szValueName [8];
            DWORD dwValueNameSize;

            std::uint64_t qwValueData;
            DWORD dwValueDataSize;

            do {
                dwValueNameSize = 8;
                dwValueDataSize = 8;
                dwError = RegEnumValue (hKeyCPU, index++, szValueName, &dwValueNameSize, NULL, &dwValueType, (LPBYTE) &qwValueData, &dwValueDataSize);
                if (dwError == ERROR_SUCCESS) {
                    if ((dwValueNameSize == 7) && (dwValueType == REG_QWORD) && (dwValueDataSize == 8)) {

                        std::uint16_t id = 0;
                        if (swscanf_s (szValueName, L"CP %hx", &id) == 1) {
                            registers [processor][id] = qwValueData;
                            result = true;
                        }
                    }
                }
            } while (dwError == ERROR_SUCCESS || dwError == ERROR_MORE_DATA);

            RegCloseKey (hKeyCPU);
        } else
            break;
    }

    return result;
}

void AArch64::SetWorkingData (const std::vector <std::map <std::uint16_t, std::uint64_t>> & data) {
    registers = data;
}

const char * AArch64::RegisterName (WORD id) noexcept {
    switch (id) {
        case Register::MIDR_EL1: return "MIDR";
        case Register::ID_AA64PFR0_EL1: return "PFR0";
        case Register::ID_AA64PFR1_EL1: return "PFR1";
        case Register::ID_AA64DFR0_EL1: return "DFR0";
        case Register::ID_AA64DFR1_EL1: return "DFR1";
        case Register::ID_AA64ISAR0_EL1: return "ISAR0";
        case Register::ID_AA64ISAR1_EL1: return "ISAR1";
        case Register::ID_AA64ISAR2_EL1: return "ISAR2";
        case Register::ID_AA64MMFR0_EL1: return "MMFR0";
        case Register::ID_AA64MMFR1_EL1: return "MMFR1";
        case Register::ID_AA64MMFR2_EL1: return "MMFR2";
        case Register::SCTLR_EL1: return "SCRLT";
        case Register::CPACR_EL1: return "CPACR";
        case Register::TTBR0_EL1: return "TTBR0";
        case Register::TTBR1_EL1: return "TTBR1";
        case Register::MAIR_EL1: return "MAIR";
    }
    return nullptr;
}

// FEAT_TO_UINT
//  - can we make this constexpr function that can be used in switch case below?
//
#define FEAT_TO_UINT(f) ((f.reg << 0) | (f.offset << 16) | (f.minimum << 24))

const char * AArch64::FeatureName (Feature feature) noexcept {
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

UINT AArch64::Heterogeneity () {
    std::set <std::map <std::uint16_t, std::uint64_t>> uniques;
    for (const auto & regmap : registers) {
        uniques.insert (regmap);
    }
    return (UINT) uniques.size ();
}

std::vector <UINT> AArch64::HeterogeneitySets () {
    std::vector <UINT> sets;
    sets.reserve (2);

    decltype (registers)::value_type prevmap;

    UINT i = 0;
    for (const auto & regmap : registers) {
        if (prevmap != regmap) {
            prevmap = regmap;
            if (i) {
                sets.push_back (i);
            }
        }
        ++i;
    }
    sets.push_back (i);
    return sets;
}

bool AArch64::Check (UINT processor, Feature feature) noexcept {
    if (processor < registers.size ()) {
        if (feature.raw == 0)
            return true;

        auto i = registers [processor].find (feature.reg);
        auto e = registers [processor].end ();

        // this is very rough hack
        // some nibbles report 0b0000 as feature not present, but some 0b1111
        // but for our purposes and current HW it's sufficient

        if (i != e) {
            auto nibble = (i->second >> feature.offset) & 0xF;
            return nibble != 0xF
                && nibble >= feature.minimum;
        }
    }
    return false;
}

namespace {
    bool AnyRegisterData () {
        for (const auto & regmap : registers) {
            if (!regmap.empty ())
                return true;
        }
        return false;
    }
}

WORD AArch64::Determine (UINT processor, Strictness strictness) noexcept {

    // TODO: IsKnownSoC -> value

    if (AnyRegisterData ()) {
        WORD match = 0x08'00;

        // TODO: rewrite to properly traverse into 9.X

        for (const auto & level : Levels) {
            for (std::size_t s = 0; s != 1 + (std::size_t) strictness; ++s) {
                for (const auto & feature : level.features [s]) {

                    if (!Check (processor, feature))
                        return match;
                }
            }
            match = level.name;
        }

        return match;
    } else 
        return 0x000;
}
