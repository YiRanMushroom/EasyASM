function OnBeforeCompile(compiler)
    compiler:GetCompilerContext().RegNameArray = {}
    compiler:GetLinkerContext().LinkAddressRequestArray = {}
    compiler:GetLinkerContext().LabelToAddressMap = {}
    compiler:GetLinkerContext().ConstantToValueMap = {}
    compiler:GetLinkerContext().LinkConstantRequestArray = {}
end

function OnBeforeLink(compiler)
end

function OnAfterLink(compiler)
end