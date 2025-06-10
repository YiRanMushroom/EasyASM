function GenerateOutput(compiler)
    local instructionBitsCount = compiler:GetBitBufferSize()
    if instructionBitsCount % 18 ~= 0 then
        return Exception.MakeCompilerImplementationError(
            "The number of bits in the instruction buffer is not a multiple of 18."
        )
    end

    if instructionBitsCount > 18 * 1024 then
        return Exception.MakeCompileError(
            "The number of bits in the instruction buffer exceeds the maximum allowed size of 18 * 1024 bits."
        )
    end

    local index = 1
    local output = "@00000000\n"
    local bitBuffer = compiler:GetBitBuffer()

    -- Padding to exactly 1024 instructions
    local bitsToAdd = 18 * 1024 - #bitBuffer
    for i = 1, bitsToAdd do
        bitBuffer:add(0)
    end

    local bit = require("bit32")

    while index <= #bitBuffer do
        local instruction = 0

        -- Read 18 bits (LSB first, i = 0 is lowest bit)
        for i = 0, 17 do
            if bitBuffer[index + i] == 1 then
                instruction = bit.bor(instruction, bit.lshift(1, i))
            end
        end

        local hexInstruction = string.format("%05X", instruction)
        output = output .. hexInstruction .. "\n"
        index = index + 18
    end

    return output
end
