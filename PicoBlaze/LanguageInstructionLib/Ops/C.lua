function ProcessCall(compiler)
    local tokenStream = compiler:GetTokenStream()
    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end

    local thisToken, conditionToWrite = Util.GetPossibleCondition(tokenStream, thisToken)

    local Address = Lib.ParseSimpleUnsigned(tokenStream, thisToken)
    if Address == nil then
        Util.WriteDummyAddress(compiler, thisToken)
    elseif Address >= 0 and Address <= 1023 then
        compiler:WriteUnsignedNumber(Address, 10)
    else
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Address value '" .. Address .. "' is out of range. Expected a value between 0 and 1023."
        )
    end
    compiler:WriteUnsignedNumber(conditionToWrite, 3)
    compiler:WriteUnsignedNumber(24, 5)
end

function ProcessCompare(compiler)
    local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the AND operation
    compiler:WriteUnsignedNumber(10, 5)
end