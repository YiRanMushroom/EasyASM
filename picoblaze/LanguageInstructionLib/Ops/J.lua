function ProcessJump(compiler)
    local tokenStream = compiler:GetTokenStream()
    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end

    local thisToken, conditionToWrite = Util.GetPossibleCondition(tokenStream, thisToken)

    Util.WriteDummyAddress(compiler, thisToken)
--     print("ProcessJump: Writing dummy address for label '" .. thisToken .. "'")
    compiler:WriteUnsignedNumber(conditionToWrite, 3)
    compiler:WriteUnsignedNumber(26, 5)
end