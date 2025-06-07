export module Core.InstructionBinary;

import std;

namespace Core {
    export class InstructionBinary {
    public:
        InstructionBinary(std::vector<bool> bits) :
            m_Bits(std::move(bits)) {
        }

        InstructionBinary(size_t perInstructionBits) :
            m_Bits(perInstructionBits, false) {
        }

        void SetBit(size_t index, bool value) {
            if (index >= m_Bits.size()) {
                throw std::out_of_range("Index out of range");
            }
            m_Bits[index] = value;
        }

        const std::vector<bool> &GetBits() const {
            return m_Bits;
        }

    private:
        std::vector<bool> m_Bits;
    };
}