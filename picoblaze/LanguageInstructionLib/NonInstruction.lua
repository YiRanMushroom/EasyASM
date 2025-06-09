function AddLabel(compiler, label)
--     print("Adding label: " .. label)
    local linkerContext = compiler:GetLinkerContext()
    if linkerContext.LabelToAddressMap[label] ~= nil then
        return Exception.MakeCompileError(
            "Label '" .. label .. "' is already defined."
        )
    end

    linkerContext.LabelToAddressMap[label] = compiler:GetBitBufferSize()
end

function ProcessNonInstruction(compiler)
    local tokenStream = compiler:GetTokenStream()
    local thisToken = tokenStream:ParseCurrent()
--     print("Processing non-instruction token: " .. tostring(thisToken))
    if thisToken == nil then
        return Exception.MakeCompilerImplementationError("No token found in the token stream.")
    end
    local nextToken = tokenStream:ParseCurrent()
--     print("Next token after non-instruction token: " .. tostring(nextToken))
    if nextToken == nil then
        return Exception.MakeCompilerImplementationError("No next token found in the token stream.")
    end

    if nextToken ~= ":" then
        return Exception.MakeCompileError(
            "Expected ':' after label, but found '" .. nextToken .. "' instead."
        )
    end

    tokenStream:SetNewLine(true)

    return AddLabel(compiler, thisToken)
end