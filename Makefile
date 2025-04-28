CXX      := g++
CXXFLAGS := -ggdb -std=c++17
# Change include path to Windows-style for GraphViz headers
CPPFLAGS := -MMD -I"C:/Program Files/Graphviz/include/graphviz"
# Change lib path to Windows-style for compilation
LDFLAGS  := -L"C:/Program Files/Graphviz/lib"

# GraphViz libraries needed
GRAPHVIZ_LIBS := -lgvc -lcgraph -lcdt

# Add runtime path to look for DLLs in executable's directory
# -Wl,--enable-runtime-pseudo-reloc helps with DLL loading
# -Wl,-rpath,'$$ORIGIN' tells the linker to look in the executable's directory
RUNTIME_PATH := -Wl,--enable-runtime-pseudo-reloc

COMPILE  := $(CXX) $(CXXFLAGS) $(CPPFLAGS)

SRCS     := ast_printer_driver.cpp prism.cpp
DEPS     := $(SRCS:.cpp=.d)


prism: expr.h stmt.h prism.o
	@$(COMPILE) prism.o -o $@ $(LDFLAGS) $(GRAPHVIZ_LIBS) $(RUNTIME_PATH)


ast_printer: expr.h stmt.h ast_printer_driver.o
	@$(COMPILE) ast_printer_driver.o -o $@ $(LDFLAGS) $(GRAPHVIZ_LIBS) $(RUNTIME_PATH)


.PHONY: clean
clean:
	rm -f *.d *.o ast_printer prism 


-include $(DEPS)

define make_test
.PHONY: $(1)
$(1):
	@make prism >/dev/null
	@echo "testing prism with $(1).prism ..."
	@./prism tests/$(1).prism | diff -u --color tests/$(1).prism.expected -;
endef


TESTS = \
test-statements \
test-statements2 \
test-statements3 \
test-statements4 \
test-statements5 \
test-statements6 \
test-control-flow \
test-control-flow2 \

$(foreach test, $(TESTS), $(eval $(call make_test,$(test))))


.PHONY: test-all
test-all:
	@for test in $(TESTS); do \
		make -s $$test; \
	done


.PHONY: test-lexing
test-lexing:
	@make prism >/dev/null
	@echo "testing prism with test-lexing.prism ..."
	@./prism tests/test-lexing.prism | diff -u --color tests/test-lexing.prism.expected -;


.PHONY: test-lexing2
test-lexing2:
	@make prism >/dev/null
	@echo "testing prism with test-lexing2.prism ..."
	@./prism tests/test-lexing2.prism | diff -u --color tests/test-lexing2.prism.expected -;


.PHONY: test-parsing
test-parsing:
	@make prism >/dev/null
	@echo "testing prism with test-parsing.prism ..."
	@./prism tests/test-parsing.prism | diff -u --color tests/test-parsing.prism.expected -;

.PHONY: test-expressions
test-expressions:
	@make prism >/dev/null
	@echo "testing prism with test-expressions.prism ..."
	@./prism tests/test-expressions.prism 2>&1 | diff -u --color tests/test-expressions.prism.expected -;


.PHONY: test-expressions2
test-expressions2:
	@make prism >/dev/null
	@echo "testing prism with test-expressions2.prism ..."
	@./prism tests/test-expressions2.prism | diff -u --color tests/test-expressions2.prism.expected -;

.PHONY: dist
dist: prism
	mkdir -p release
	mkdir -p release/lib
	mkdir -p release/images

	cp prism.exe release/
	cp "C:\Program Files\Graphviz\bin\dot.exe" release/lib/

	# GraphViz core DLLs
	cp "C:\Program Files\Graphviz\bin\gvc.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gvc++.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\cgraph.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\cgraph++.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\cdt.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\xdot.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\pathplan.dll" release/lib/

	# GraphViz plugins
	cp "C:\Program Files\Graphviz\bin\gvplugin_core.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_dot_layout.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_neato_layout.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_gd.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_gdiplus.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_pango.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_vt.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_webp.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_kitty.dll" release/lib/

	# Tcl support
	cp "C:\Program Files\Graphviz\bin\gdtclft.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\tcl86t.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\tcldot.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\tcldot_builtin.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\tclplan.dll" release/lib/

	# Pango and font rendering
	cp "C:\Program Files\Graphviz\bin\pango-1.0-0.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\pangocairo-1.0-0.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\pangoft2-1.0-0.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\pangowin32-1.0-0.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\harfbuzz.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\freetype.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\fontconfig-1.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\fribidi-0.dll" release/lib/

	# GLib and related
	cp "C:\Program Files\Graphviz\bin\glib-2.0-0.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gobject-2.0-0.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gmodule-2.0-0.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\gio-2.0-0.dll" release/lib/

	# Image handling
	cp "C:\Program Files\Graphviz\bin\cairo-2.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\libgd.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\libpng16.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\jpeg62.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\tiff.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\libwebp.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\libsharpyuv.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\pixman-1-0.dll" release/lib/

	# Compression and utilities
	cp "C:\Program Files\Graphviz\bin\zlib1.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\liblzma.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\bz2.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\brotlicommon.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\brotlidec.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\pcre2-8.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\libexpat.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\iconv-2.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\intl-8.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\ffi-8.dll" release/lib/

	# Other GraphViz libraries
	cp "C:\Program Files\Graphviz\bin\gts.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\getopt.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\ANN.dll" release/lib/

	# Visual C++ Runtime DLLs
	cp "C:\Program Files\Graphviz\bin\vcruntime140.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\vcruntime140_1.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140_1.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140_2.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140_atomic_wait.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140_codecvt_ids.dll" release/lib/
	cp "C:\Program Files\Graphviz\bin\concrt140.dll" release/lib/

	# MinGW runtime DLLs (already included)
	cp "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll" release/lib/
	cp "C:\msys64\mingw64\bin\libstdc++-6.dll" release/lib/
	cp "C:\msys64\mingw64\bin\libwinpthread-1.dll" release/lib/

	# GraphViz configuration
	cp "C:\Program Files\Graphviz\bin\config6" release/lib/
