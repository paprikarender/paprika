local function matte(r, g, b)
    shaderGroupBegin()
	parameter("color Cs", {r, g, b})
    shader("surface", "matte", "layer1")
    shaderGroupEnd();
end

local function ward(r, g, b, ax, ay)
    shaderGroupBegin()
	parameter("color Cs", {r, g, b})
	parameter("float ax", ax)
	parameter("float ay", ay)
    shader("surface", "ward", "layer1")
    shaderGroupEnd();
end

lookAt(3.69558, -3.46243, 3.25463, 3.04072, -2.85176, 2.80939, -0.317366, 0.312466, 0.895346)

camera("perspective", "int[2] resolution", {683, 512}, "float fov", 28.8415)

world()

shaderGroupBegin()
parameter("float Kb", 3)
parameter("string filename", "envmap.exr")
shader("surface", "envmap", "layer1")
shaderGroupEnd()
background()

shaderGroupBegin()
parameter("float scale_s", 16)
parameter("float scale_t", 16)
parameter("color Ca", { 0.6, 0.6, 0.6 })
parameter("color Cb", { 0.3, 0.3, 0.3 })
shader("surface", "checkerboard", "tex")
shader("surface", "matte", "layer1")
connectShaders("tex", "Cout", "layer1", "Cs")
shaderGroupEnd()

pushTransform()
rotate(-4.3, 0, 0, 1)
appendTransform(3.38818, 4.06354, 0, 0, -4.06354, 3.38818, 0, 0, 0, 0, 5.29076, 0, -1.74958, 1.43683, -0.0120714, 1)
input("1.trimesh")
popTransform()

matte(0.3, 0.3, 0.3)

pushTransform()
appendTransform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0.0252155, 1)
input("2.trimesh")
popTransform()

ward(0.5, 0.5, 0.8, 0.02, 0.02)

pushTransform()
appendTransform(0.614046, -0.614047, 0, 0, 0.614047, 0.614046, 0, 0, 0, 0, 0.868393, 0, 0, 0, 1.02569, 1)
translate(0, 0, 0.01)
input("3.trimesh")
popTransform()

render()
