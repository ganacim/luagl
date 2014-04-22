local m = require "glviewer"

function du( t, p )
    local R = 1
    local r = 0.5
    --{-(R + r Cos[p]) Sin[t], (R + r Cos[p]) Cos[t], 0}
    return {-(R + r*math.cos(p))*math.sin(t), (R + r*math.cos(p))*math.cos(t), 0}
end

function dv( t, p, R, r )
    local R = 1
    local r = 0.5
    return {-r*math.cos(t)*math.sin(p), -r*math.sin(p)*math.sin(t), r*math.cos(p)}
end

function cross( v0, v1 )
    local a, b, c = v0[1], v0[2], v0[3]
    local d, e, f = v1[1], v1[2], v1[3]
    return {-c*e + b*f, c*d - a*f, -b*d + a*e}
end

function normalize( v0 )
    local n = math.sqrt( v0[1]*v0[1] + v0[2]*v0[2] + v0[3]*v0[3] )
    return { v0[1]/n, v0[2]/n, v0[3]/n }
end

function normal( t, p )
    local n = normalize( cross( du(t, p), dv(t, p) ) )
    return n
end

function torus( theta, phi )
    local R = 1
    local r = 0.5
    local p = { (R + r*math.cos(phi))*math.cos(theta),
                (R + r*math.cos(phi))*math.sin(theta),
                r*math.sin(phi) }
    return p
end


function add_tri( p0, p1, p2, n0, n1, n2 )
    m.add_point( p0[1], p0[2], p0[3], n0[1], n0[2], n0[3] )
    m.add_point( p1[1], p1[2], p1[3], n1[1], n1[2], n1[3] )
    m.add_point( p2[1], p2[2], p2[3], n2[1], n2[2], n2[3] )
end

local n=100
local du = 2*math.pi/n
local dv = 2*math.pi/n
for u=0, 2*math.pi, du do
    for v=0, 2*math.pi, dv do
        --print( normal(u, v) )
        local p0, n0 = torus( u, v ), normal( u, v )
        local p1, n1 = torus( u+du, v ), normal( u+du, v )
        local p3, n3 = torus( u+du, v+dv ), normal( u+du, v+dv );
        local p2, n2 = torus( u, v+dv ), normal( u, dv+v )
        add_tri( p0, p1, p3, n0, n1, n3 )
        add_tri( p0, p3, p2, n0, n3, n2 )
    end
end

m.show()
