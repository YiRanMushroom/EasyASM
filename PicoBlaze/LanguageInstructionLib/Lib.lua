function Lib.ParseSimpleRegister(tokenStream, str)
    local lower = Lib.ToLowerCase(str)
    if (#lower ~= 2 or lower:sub(1,1) ~= 's') then
        return nil
    end

    local second = lower:sub(2,2)
    if not((second >= '0' and second <='9') or(second >= 'a' and second <= 'f')) then
        return nil
    end

    return tonumber(second, 16)
end

function Lib.ParseRegister(compiler, str)
    local tokenStream = compiler:GetTokenStream()
    local RegNameArray = compiler:GetCompilerContext().RegNameArray
    result = Lib.ParseSimpleRegister(tokenStream, str)
    if result ~= nil then
        if RegNameArray[result] ~= nil then
            return nil
        end

        return result
    else
        for i = 0,  15 do
            if RegNameArray[i] == str then
                return i
            end
        end

        return nil
    end
end

function Lib.ParseSimpleUnsigned(tokenStream, str) -- 8 bit unsigned immediate value
    if str:sub(1,2) == "0x" or str:sub(1,2) == "0X" then
        print("Warning: Immediate value should not include '0x' prefix: " .. str)
        str = str:sub(3)
    end

    return tonumber(str, 16)
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
        "Compiler Implementation Error " .. position .. ": " .. message .. ' This is a bug in the compiler implementation, please report this to your compiler vendor.'
    )
end