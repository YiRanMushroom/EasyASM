export module Core.ASMElements.InstructionBinary;

import std;
import Vendor.sol;

namespace Core::ASMElements {
    export class InstructionBinary {
    public:
        InstructionBinary(std::vector<bool> bits) :
            m_Bits(std::move(bits)) {
        }

        InstructionBinary(size_t perInstructionBits) :
            m_Bits(perInstructionBits, false) {
        }

        void SetBit(size_t index, bool value);

        const std::vector<bool> &GetBits() const;

        static void AddLibToState(sol::state& state);

    private:
        std::vector<bool> m_Bits;
    };
}