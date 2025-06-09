function OnBeforeCompile(compiler)
    compiler:GetCompilerContext().RegNameArray = {}
    compiler:GetLinkerContext().LinkAddressRequestArray = {}
    compiler:GetLinkerContext().LabelToAddressMap = {}
end

function OnBeforeLink(compiler)
end

function OnAfterLink(compiler)
end