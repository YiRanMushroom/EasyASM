module Core.ASMElements.InstructionBinary;

import std;

namespace Core::ASMElements {
    void InstructionBinary::SetBit(size_t index, bool value) {
        if (index >= m_Bits.size()) {
            throw std::out_of_range("Index out of range");
        }
        m_Bits[index] = value;
    }

    const std::vector<bool> & InstructionBinary::GetBits() const {
        return m_Bits;
    }

    void InstructionBinary::AddLibToState(sol::state &state) {
        state.new_usertype<InstructionBinary>(
            "InstructionBinary",
            sol::constructors<InstructionBinary(std::vector<bool>), InstructionBinary(size_t)>(),
            "SetBit", &InstructionBinary::SetBit,
            "GetBits", &InstructionBinary::GetBits
        );
    }
}
