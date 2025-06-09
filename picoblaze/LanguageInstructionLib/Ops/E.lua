function ProcessEnable(compiler)
    local tokenStream = compiler:GetTokenStream()
    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end

    if Lib.ToLowerCase(thisToken) ~= "interrupt" then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Expected 'interrupt' keyword after 'enable', but found '" .. thisToken .. "'."
        )
    end

    compiler:WriteUnsignedNumber(245761, 18)
end