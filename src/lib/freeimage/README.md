# FreeImage 3.7.0

Sources from the FreeImage 3.7.0 source distribution (Source/), with line
endings converted to LF. FreeImage is dual-licensed under the GPL and the
FreeImage Public License; see http://freeimage.sourceforge.net/license.html.

Tornado.exe links a trimmed FreeImage: only the PNG and JPEG plugins are
registered. Wanako built its C++ code with the engine's flags (`/O2 /GX`),
not FreeImage's own project settings, while its bundled C libraries
(zlib, libpng, libjpeg; see `src/lib/`) were built with `/O1`.

Changes from upstream, each needed to match:

- `FreeImage/Plugin.cpp`: `FreeImage_Initialise` registers only `InitJPEG`
  and `InitPNG`. Plugin.cpp and FreeImage.cpp are built without `/DWIN32`,
  which drops the external `.fip` plugin loader, `FreeLibrary` in
  `~PluginList` and `FreeImage_RegisterExternalPlugin`; the exe has none of
  them.
