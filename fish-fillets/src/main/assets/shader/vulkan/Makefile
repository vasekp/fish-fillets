ALL: pixel.spv copy.spv alpha.spv reverse.spv flat.spv disintegrate.spv wavy-image.spv wavy-text.spv title.spv zx.spv button.spv

%.spv: %.frag
	glslc -o $@ $<

pixel.spv: pixel.vert
	glslc -o $@ $<

.PHONY: ALL
