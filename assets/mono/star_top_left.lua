local process = require("wf.api.v1.process")
local superfamiconv = require("wf.api.v1.process.tools.superfamiconv")

for i, file in pairs(process.inputs(".png")) do
	local tileset = superfamiconv.convert_tileset(
		file,
		superfamiconv.config()
			:mode("ws"):bpp(2)
			:color_zero("#ffffff")
			:no_discard():no_flip()
	)
	process.emit_symbol("gfx_mono_" .. process.symbol(file), tileset.tiles)
end
