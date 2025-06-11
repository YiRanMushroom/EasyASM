function ProcessOr(compiler)
    local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the AND operation
    compiler:WriteUnsignedNumber(6, 5)
end

function ProcessOutput(compiler)
    local callResult = Util.WriteAddressImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the AND operation
    compiler:WriteUnsignedNumber(22, 5)
end