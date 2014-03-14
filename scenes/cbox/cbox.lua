local function matte(r, g, b)
    shaderGroupBegin()
	parameter("color Cs", {r, g, b})
    shader("surface", "matte", "layer1")
    shaderGroupEnd();
end

local function emitter(r, g, b, power)
    shaderGroupBegin()
    parameter("color Cs", {r, g, b})
    parameter("float power", power)
    shader("surface", "emitter", "layer1")
    shaderGroupEnd()
end

local function quad(x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3, emissive)
    parameter("vertex point P", {x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3})
    parameter("int emissive", emissive or 0)
    mesh("linear", {4}, {0, 1, 2, 3})
end

local function largebox()
    local P = {
        423, 330, 247,
        265, 330, 296,
        314, 330, 456,
        472, 330, 406,
        423, 0, 247,
        423, 330, 247,
        472, 330, 406,
        472, 0, 406,
        472, 0, 406,
        472, 330, 406,
        314, 330, 456,
        314, 0, 456,
        314, 0, 456,
        314, 330, 456,
        265, 330, 296,
        265, 0, 296,
        265, 0, 296,
        265, 330, 296,
        423, 330, 247,
        423, 0, 247,
        472, 0, 406,
        314, 0, 456,
        265, 0, 296,
        423, 0, 247 }

    parameter("vertex point P", P)

    local nverts = { 4, 4, 4, 4, 4, 4 }
    local verts = { 0, 1, 2, 3,
                    4, 5, 6, 7,
                    8, 9, 10, 11,
                    12, 13, 14, 15,
                    16, 17, 18, 19,
                    20, 21, 22, 23 }

    mesh("linear", nverts, verts)		
end

local function smallbox()
    local P = {
        130, 165, 65,
        82, 165, 225,
        240, 165, 272,
        290, 165, 114,
        290, 0, 114,
        290, 165, 114,
        240, 165, 272,
        240, 0, 272,
        130, 0, 65,
        130, 165, 65,
        290, 165, 114,
        290, 0, 114,
        82, 0, 225,
        82, 165, 225,
        130, 165, 65,
        130, 0, 65,
        240, 0, 272,
        240, 165, 272,
        82, 165, 225,
        82, 0, 225,
        290, 0, 114,
        240, 0, 272,
        82, 0, 225,
        130, 0, 65 }

    parameter("vertex point P", P)

    local nverts = { 4, 4, 4, 4, 4, 4 }
    local verts = { 0, 1, 2, 3,
                    4, 5, 6, 7,
                    8, 9, 10, 11,
                    12, 13, 14, 15,
                    16, 17, 18, 19,
                    20, 21, 22, 23 }

    mesh("linear", nverts, verts)		
end



lookAt(278, 273, -800, 278, 273, -799, 0, 1, 0)

camera("perspective", "int[2] resolution", {512, 512}, "float fov", 39.3077)

world()

matte(0.75, 0.25, 0.25)
quad(552.8, 0, 0, 549.6, 0, 559.2, 556, 548.8, 559.2, 556, 548.8, 0)       -- green wall

matte(0.25, 0.75, 0.25)
quad(0, 0, 559.2, 0, 0, 0, 0, 548.8, 0, 0, 548.8, 559.2)                   -- red wall

matte(0.75, 0.75, 0.75)
quad(552.8, 0, 0, 0, 0, 0, 0, 0, 559.2, 549.6, 0, 559.2)                   -- floor
quad(556, 548.8, 0, 556, 548.8, 559.2, 0, 548.8, 559.2, 0, 548.8, 0)       -- ceiling
quad(549.6, 0, 559.2, 0, 0, 559.2, 0, 548.8, 559.2, 556, 548.8, 559.2)     -- back

matte(0.75, 0.75, 0.75)
largebox()
smallbox()

emitter(1.0, 0.8, 0.6, 2500000)
quad(343, 548.3, 227, 343, 548.3, 332, 213, 548.3, 332, 213, 548.3, 227, 1)

render()
