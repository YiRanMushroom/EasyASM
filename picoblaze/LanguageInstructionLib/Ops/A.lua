function ProcessAdd(compiler)
    local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the ADD operation
    compiler:WriteUnsignedNumber(12, 5)
end

