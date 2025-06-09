function ParseAddress(compiler)
    local tokenStream = compiler:GetTokenStream()
    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local address = Lib.ParseSimpleUnsigned(tokenStream, thisToken)
    if address == nil or address < 0 or address > 1023 then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Invalid address value: '" .. thisToken .. "'. Expected a value between 0 and 1023."
        )
    end
    local BitBufferSize = compiler:GetBitBufferSize()
    if 18 * address < BitBufferSize then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Address value '" .. address .. "' is too small for the current bit buffer size of " .. BitBufferSize .. "."
        )
    end
    compiler:WriteUnsignedNumber(0, 18 * address - BitBufferSize)
end