#ifndef _RISCV_CAM_H
#define _RISCV_CAM_H

#include <cassert>
#include <iostream>
#include <array>
#include <vector>
#include "extension.h"

class cam_t : public extension_t {
protected:

  typedef std::pair<reg_t, reg_t> key_type;
  typedef reg_t data_type;
  typedef std::pair<key_type, data_type> value_type;
  typedef std::array<value_type, 128> state_t;
  typedef state_t::size_type state_ix_t;
  typedef std::multimap<key_type, state_ix_t> map_t;

  state_t camState;
  map_t camMap;

public:

  const char* name() { return "cam"; }

  std::vector<insn_desc_t> get_instructions();
  std::vector<disasm_insn_t*> get_disasms();

  reg_t camlk0(reg_t xs1, int64_t xi) {
    const state_ix_t ix = xs1 + xi;
    const reg_t result = camState.at(ix).first.first;
    std::cout << "cam[" << ix << "].k0 => " << result << std::endl;
    return result;
  }

  reg_t camlk1(reg_t xs1, int64_t xi) {
    const state_ix_t ix = xs1 + xi;
    const reg_t result = camState.at(ix).first.second;
    std::cout << "cam[" << ix << "].k1 => " << result << std::endl;
    return result;
  }

  reg_t camlv(reg_t xs1, int64_t xi) {
    const state_ix_t ix = xs1 + xi;
    const data_type result = camState.at(ix).second;
    std::cout << "cam[" << (xs1 + xi) << "].v  => " << result << std::endl;
    return result;
  }

  void camsk0(reg_t xs1, reg_t xs2, int64_t xi) {
    const state_ix_t ix = xs1 + xi;
    removeMapping(ix);
    camState.at(ix).first.first = xs2;
    addMapping(ix);
    std::cout << "cam[" << ix << "].k0 <= " << xs2 << std::endl;
  }

  void camsk1(reg_t xs1, reg_t xs2, int64_t xi) {
    const state_ix_t ix = xs1 + xi;
    removeMapping(ix);
    camState.at(ix).first.second = xs2;
    addMapping(ix);
    std::cout << "cam[" << ix << "].k1 <= " << xs2 << std::endl;
  }

  void camsv(reg_t xs1, reg_t xs2, int64_t xi) {
    const state_ix_t ix = xs1 + xi;
    removeMapping(ix);
    camState.at(ix).second = xs2;
    addMapping(ix);
    std::cout << "cam[" << ix << "].v  <= " << xs2 << std::endl;
  }

  reg_t cam(reg_t xs1, reg_t xs2, uint64_t ignoreBits) {
    const reg_t mask = ((reg_t)-1 << ignoreBits);
    const reg_t prefixLower = xs2 & mask;
    const reg_t prefixUpper = xs2 | ~mask;
    const reg_t suffix = xs2 & ~mask;
    const key_type keyLowerBound(xs1, prefixLower);
    const key_type keyUpperBound(xs1, prefixUpper);

    // Find a range of entries in the map that could potentially match the prefix.
    map_t::iterator lb = camMap.lower_bound(keyLowerBound);
    const map_t::iterator ub = camMap.upper_bound(keyUpperBound);

    bool foundMatch = false;
    state_ix_t ix = 0;

    // Iterate over the potentially matching rows and take the actual matching row with the lowest ix.
    while (lb != ub) {
      const reg_t k0 = (*lb).first.first;
      const reg_t k1 = (*lb).first.second;
      if (k0 == xs1 && (k1 & mask) == prefixLower) {
        if (!foundMatch) {
          std::cout << "cam(" << xs1 << ", " << xs2 << " & 0x" << std::hex << mask << ").v => " << std::endl;
          ix = (*lb).second;
        } else {
          ix = std::min(ix, (*lb).second);
        }
        foundMatch = true;
        std::cout << "    matches ix=" << (*lb).second << " (" << (*lb).first.first << ", " << (*lb).first.second << ") -> " << camState.at((*lb).second).second << std::endl;
      }
      ++lb;
    }

    if (foundMatch) {
      reg_t result = camState.at(ix).second;
      reg_t resultWithSuffix = (result & mask) | suffix;

      std::cout << "    minimum ix=" << ix << " value=" << result
                << " => (" << result << " & 0x" << std::hex << mask << ") | 0x" << suffix << " = " << resultWithSuffix << std::endl;

      return resultWithSuffix;
    }
    else {
      // Key is not in the cam.
      std::cout << "cam(" << xs1 << ", " << xs2 << " & 0x" << std::hex << mask << ").v => null" << std::endl;
      return 0;
    }
  }

  void camx() {
    illegal_instruction();
  }

protected:

  void removeMapping(const state_ix_t ix) {
    const value_type row = camState.at(ix);
    if (row.second != 0) {
      // CAM row does not contain a zero value, so the multimap should have an entry pointing to this row.
      std::cout << "cam[" << ix << "] no longer contains (" << row.first.first << ", " << row.first.second << ") -> " << row.second << std::endl;
      std::pair<map_t::iterator, map_t::iterator> range = camMap.equal_range(row.first);
      while (range.first != range.second) {
        if ((*(range.first)).second == ix) {
          camMap.erase(range.first);
          return;
        }
        ++range.first;
      }
      // Failed to find the expected entry.
      assert(false);
    }
  }

  void addMapping(const state_ix_t ix) {
    const value_type row = camState.at(ix);
    if (row.second != 0) {
      // CAM row does not contain a zero value. Create an entry in camMap pointing to this row.
      camMap.insert(map_t::value_type(row.first, ix));
      std::cout << "cam[" << ix << "] now contains (" << row.first.first << ", " << row.first.second << ") -> " << row.second << std::endl;
    }
  }

};

#endif /* _RISCV_CAM_H */
