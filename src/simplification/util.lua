local M = {}

function M.to_string(expression)
    local str = {}
    for _, summand in ipairs(expression) do
        table.insert(str, table.concat(summand, " * "))
    end
    return table.concat(str, " + ")
end

return M
