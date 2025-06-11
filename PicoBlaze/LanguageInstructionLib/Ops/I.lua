function ProcessInput(compiler)
    local callResult = Util.WriteAddressImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the AND operation
    compiler:WriteUnsignedNumber(2, 5)
end