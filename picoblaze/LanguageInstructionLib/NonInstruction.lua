function AddLabel(compiler, label)
    local linkerContext = compiler:GetLinkerContext()
    if linkerContext.LabelToAddressMap[label] ~= nil then
        return Exception.MakeCompileErrorWithLocation(
            compiler:GetTokenStream(),
            "Label '" .. label .. "' is already defined."
        )
    end

    linkerContext.LabelToAddressMap[label] = compiler:GetBitBufferSize() / 18
end

function ProcessNonInstruction(compiler)
    local tokenStream = compiler:GetTokenStream()
    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompilerImplementationErrorWithLocation(
            tokenStream,
            "No token found in the token stream."
        )
    end
    local nextToken = tokenStream:ParseCurrent()
    if nextToken == nil then
        return Exception.MakeCompilerImplementationErrorWithLocation(
            tokenStream,
            "No next token found in the token stream."
        )
    end

    if nextToken ~= ":" then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Expected ':' after label, but found '" .. nextToken .. "' instead."
        )
    end

    tokenStream:SetNewLine(true)

    return AddLabel(compiler, thisToken)
end