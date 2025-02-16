local process = require("wf.api.v1.process")
local superfamiconv = require("wf.api.v1.process.tools.superfamiconv")
local zx0 = require("wf.api.v1.process.tools.zx0")

for i, file in pairs(process.inputs(".png")) do
	local tilemap = superfamiconv.convert_tilemap(
		file,
		superfamiconv.config()
			:mode("ws"):bpp(2)
			:tile_base(0):palette_base(0)
	)
	tilemap.tiles = zx0.compress(tilemap.tiles)
	tilemap.map = zx0.compress(tilemap.map)
	process.emit_symbol("gfx_mono_" .. process.symbol(file), tilemap)
end
