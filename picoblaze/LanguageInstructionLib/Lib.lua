function Lib.ParseRegister(tokenStream, str)
    if str:sub(1, 1) ~= 's' then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Invalid register name: " .. str
        )
    end

    local regNum = tonumber(str:sub(2), 16)
    if regNum == nil or regNum < 0 or regNum > 15 then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Invalid register number: " .. tostring(regNum) .. " in " .. str
        )
    end

    return regNum
end

function Lib.ParseUnsigned(tokenStream, str) -- 8 bit unsigned immediate value
    if str:sub(1,2) == "0x" or str:sub(1,2) == "0X" then
        print("Warning: Immediate value should not include '0x' prefix: " .. str)
        str = str:sub(3)
    end

    local value = tonumber(str, 16)
    if value == nil or value < 0 or value > 255 then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "Invalid unsigned immediate value: " .. tostring(value) .. " when parsing token " .. str)
    end

    return value
end

function Exception.MakeCompileErrorWithLocation(tokenStream, message)
    local position = tokenStream:GetApproxCurrentLocation()
    return Exception.MakeCompileError(
        "Compile Error " .. position .. ": " .. message
    )
end

function Exception.MakeLinkErrorWithLocation(tokenStream, message)
    local position = tokenStream:GetApproxCurrentLocation()
    return Exception.MakeLinkError(
        "Link Error " .. position .. ": " .. message
    )
end

function Exception.MakeCompilerImplementationErrorWithLocation(tokenStream, message)
    local position = tokenStream:GetApproxCurrentLocation()
    return Exception.MakeCompilerImplementationError(
        "Compiler Implementation Error " .. position .. ": " .. message
    )
end