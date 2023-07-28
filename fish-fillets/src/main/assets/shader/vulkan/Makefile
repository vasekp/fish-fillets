ALL: pixel.spv copy.spv alpha.spv reverse.spv flat.spv disintegrate.spv wavy-image.spv wavy-text.spv title.spv zx.spv button.spv arrow.vert.spv arrow.frag.spv

%.spv: %.frag
	glslc -o $@ $<

pixel.spv: pixel.vert
	glslc -o $@ $<

arrow.vert.spv: arrow.vert
	glslc -o $@ $<

arrow.frag.spv: arrow.frag
	glslc -o $@ $<

.PHONY: ALL
