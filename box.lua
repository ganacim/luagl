local m = require "glviewer"
local t = {
    {-1, -1, -1, 1},
    {1, -1, -1, 1},
    {1,  1, -1, 1},
    {-1,  1, -1, 1},
    {-1, -1,  1, 1},
    { 1, -1,  1, 1},
    { 1,  1,  1, 1},
    {-1,  1,  1, 1},
}

local i = {
    {0, 3, 2},
    {0, 2, 1},
    {0, 1, 5},
    {0, 5, 4},
    {0, 4, 7},
    {0, 7, 3},
    {6, 5, 1},
    {6, 1, 2},
    {6, 2, 3},
    {6, 3, 7},
    {6, 7, 4},
    {6, 4, 5}
}

for k,v in ipairs(i) do
    local il = v
    for w, n in ipairs(il) do
        local p = t[n+1]
        m.add_point( p[1], p[2], p[3], p[1], p[2], p[3] )
    end
end
m.show()
