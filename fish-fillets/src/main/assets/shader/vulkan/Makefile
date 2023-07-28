ALL: pixel.spv copy.spv alpha.spv flat.spv

%.spv: %.frag
	glslc -o $@ $<

pixel.spv: pixel.vert
	glslc -o $@ $<

.PHONY: ALL
