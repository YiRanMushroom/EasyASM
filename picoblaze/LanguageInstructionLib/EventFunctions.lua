function OnBeforeCompile(compiler)
    print("OnBeforeCompile called")
end

function OnBeforeLink(compiler)
    print("OnBeforeLink called")
end

function OnAfterLink(compiler)
    print("OnAfterLink called")
end