# chrtool — planned CHR asset tool

Target features:
- decode 2bpp NES pattern-table bytes to PNG
- encode a constrained indexed image to CHR tiles
- display 8x8/8x16 sprite previews and palettes
- deduplicate tiles and report tile count/bank capacity
- preserve source dimensions/palette metadata where supported

The NES uses 2bpp tile planes and a separate palette mapping. A converter must validate palette/index rules instead of silently quantizing art in an unpredictable way.
