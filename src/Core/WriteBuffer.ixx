export module Core.WriteBuffer;

import std;
import Core.Lib;
import Core.Exceptions;

namespace Core {
    // export class WriteBuffer {
    // public:
    //     WriteBuffer() = default;
    //
    //     std::vector<bool>& GetBuffer() {
    //         return m_BitBuffer;
    //     }
    //
    //     void WriteBit(bool bit) {
    //         m_BitBuffer.push_back(bit);
    //     }
    //
    //     void WriteBits(const std::vector<bool>& bits) {
    //         m_BitBuffer.insert(m_BitBuffer.end(), bits.begin(), bits.end());
    //     }
    //
    //     void WriteNumber(uint64_t number, size_t bits) {
    //         if (bits > 64) {
    //             throw Exceptions::CompileError(
    //                 std::format("Compile Error: A")
    //             );
    //         }
    //     }
    //
    // private:
    //
    // };
}