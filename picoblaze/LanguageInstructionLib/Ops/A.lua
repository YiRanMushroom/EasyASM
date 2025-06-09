function ProcessAdd(compiler)
    local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the ADD operation
    compiler:WriteUnsignedNumber(12, 5)
end

function ProcessAddCarry(compiler)
    local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the ADD operation with carry
    compiler:WriteUnsignedNumber(13, 5)
end

function ProcessAnd(compiler)
    local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the AND operation
    compiler:WriteUnsignedNumber(5, 5)
end