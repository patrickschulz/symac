local ratfun = {
    num = { { "x", "R" } },
    den = { { "x", "a" }, { "x", "R", "C" } }
}

local function normalize(ratfun)
    local common = {}
    for _, coefficient in ipairs(ratfun.num) do
        table.insert(common, coefficient)
    end
    for _, coefficient in ipairs(ratfun.den) do
        table.insert(common, coefficient)
    end

    -- search common factors
    local factors = {}
    for i, coefficient in ipairs(common) do
        for _, factor in ipairs(coefficient) do
            local found = true
            for ii, othercoefficient in ipairs(common) do
                if ii ~= i then
                    local innerfound = false
                    for _, otherfactor in ipairs(othercoefficient) do
                        if otherfactor == factor then
                            innerfound = true
                        end
                    end
                    if not innerfound then
                        found = false
                    end
                end
            end
            if found then
                table.insert(factors, factor)
            end
        end
    end

    -- remove common factors
    for _, commonfactor in ipairs(factors) do
        for ci, coefficient in ipairs(ratfun.num) do
            local idx = {}
            for i, factor in ipairs(coefficient) do
                if factor == commonfactor then
                    table.insert(idx, i)
                end
            end
            if #coefficient == #idx then
                ratfun.num[ci] = { "1" }
            else
                for _, i in ipairs(idx) do
                    table.remove(ratfun.num[ci], i)
                end
            end
        end
        for ci, coefficient in ipairs(ratfun.den) do
            local idx = {}
            for i, factor in ipairs(coefficient) do
                if factor == commonfactor then
                    table.insert(idx, i)
                end
            end
            if #coefficient == #idx then
                ratfun.den[ci] = { "1" }
            else
                for _, i in ipairs(idx) do
                    table.remove(ratfun.den[ci], i)
                end
            end
        end
    end
end

normalize(ratfun)

print(string.format("H(s) = %s / (%s + s * %s)", table.concat(ratfun.num[1], " * "), table.concat(ratfun.den[1], " * "), table.concat(ratfun.den[2], " * ")))

print(string.format("integrate(|H(jw)|^2, f, 0, inf) = %s^2 / (4 * %s * %s)", table.concat(ratfun.num[1], " * "), table.concat(ratfun.den[1], " * "), table.concat(ratfun.den[2], " * ")))
--print(string.format("%s * %s^2 / (4 * %s)", "4 k T / R", table.concat(ratfun.num[1], " * "), table.concat(ratfun.den[2], " * ")))
