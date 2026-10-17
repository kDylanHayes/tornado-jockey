# FreeImage 3.7.0

Sources from the FreeImage 3.7.0 source distribution (Source/), with line
endings converted to LF. FreeImage is dual-licensed under the GPL and the
FreeImage Public License; see http://freeimage.sourceforge.net/license.html.

Tornado.exe links a trimmed FreeImage: only the PNG and JPEG plugins are
registered. Wanako built its C++ code with the engine's flags (`/O2 /GX`),
not FreeImage's own project settings, while its bundled C libraries
(zlib, libpng, libjpeg; see `src/lib/`) were built with `/O1`.
