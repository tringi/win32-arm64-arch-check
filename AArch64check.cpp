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

bool AArch64::Initialize (const std::vector <std::map <std::uint16_t, std::uint64_t>> & data) {
    registers = data;
    return true;
}

std::size_t AArch64::Heterogeneity () {
    std::set <std::map <std::uint16_t, std::uint64_t>> uniques;
    for (const auto & regmap : registers) {
        uniques.insert (regmap);
    }
    return uniques.size ();
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

    bool ValidateLevel (const AArch64::Level & level, UINT processor, AArch64::Strictness strictness) {
        for (std::size_t s = 0; s != 1 + (std::size_t) strictness; ++s) {
            for (const auto & feature : level.features [s]) {
                if (!Check (processor, feature))
                    return false;
            }
        }
        return true;
    }
}

WORD AArch64::Determine (UINT processor, Strictness strictness) noexcept {

    // TODO: IsKnownSoC -> value

    if (AnyRegisterData ()) {
        WORD match = 0x08'00;

        // TODO: rewrite to properly traverse into 9.X

        for (const auto & level : Levels) {
            if (!ValidateLevel (level, processor, strictness))
                return match;

            match = level.name;
        }

        return match;
    } else 
        return 0x000;
}
