local M = {}

local tablex = require "pl.tablex"

function M.calculate_weights(expression, weightmap)
    local weights = {}
    for index, summand in ipairs(expression) do
        local weight = 0
        for _, factor in ipairs(summand) do
            weight = weight + weightmap[factor]
        end
        table.insert(weights, index, weight)
    end
    return weights
end

function M.calculate_indices_to_keep(weights)
    local maxweight = -1
    local tokeep
    for index, weight in ipairs(weights) do
        if weight > maxweight then
            tokeep = {}
            table.insert(tokeep, index)
            maxweight = weight
        elseif weight == maxweight then
            table.insert(tokeep, index)
        end
    end
    return tokeep
end

function M.create_new_expression(expression, tokeep)
    local new= {}
    for index, summand in ipairs(expression) do
        if tablex.find(tokeep, index) then
            table.insert(new, summand)
        end
    end
    return new
end

return M
