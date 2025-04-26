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
	mkdir -p prism_app
	mkdir -p prism_app/lib
	mkdir -p prism_app/images

	cp prism.exe prism_app/
	cp "C:\Program Files\Graphviz\bin\dot.exe" prism_app/lib/

	# GraphViz core DLLs
	cp "C:\Program Files\Graphviz\bin\gvc.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gvc++.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\cgraph.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\cgraph++.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\cdt.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\xdot.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\pathplan.dll" prism_app/lib/

	# GraphViz plugins
	cp "C:\Program Files\Graphviz\bin\gvplugin_core.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_dot_layout.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_neato_layout.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_gd.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_gdiplus.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_pango.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_vt.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_webp.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gvplugin_kitty.dll" prism_app/lib/

	# Tcl support
	cp "C:\Program Files\Graphviz\bin\gdtclft.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\tcl86t.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\tcldot.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\tcldot_builtin.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\tclplan.dll" prism_app/lib/

	# Pango and font rendering
	cp "C:\Program Files\Graphviz\bin\pango-1.0-0.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\pangocairo-1.0-0.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\pangoft2-1.0-0.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\pangowin32-1.0-0.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\harfbuzz.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\freetype.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\fontconfig-1.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\fribidi-0.dll" prism_app/lib/

	# GLib and related
	cp "C:\Program Files\Graphviz\bin\glib-2.0-0.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gobject-2.0-0.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gmodule-2.0-0.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\gio-2.0-0.dll" prism_app/lib/

	# Image handling
	cp "C:\Program Files\Graphviz\bin\cairo-2.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\libgd.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\libpng16.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\jpeg62.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\tiff.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\libwebp.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\libsharpyuv.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\pixman-1-0.dll" prism_app/lib/

	# Compression and utilities
	cp "C:\Program Files\Graphviz\bin\zlib1.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\liblzma.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\bz2.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\brotlicommon.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\brotlidec.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\pcre2-8.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\libexpat.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\iconv-2.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\intl-8.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\ffi-8.dll" prism_app/lib/

	# Other GraphViz libraries
	cp "C:\Program Files\Graphviz\bin\gts.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\getopt.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\ANN.dll" prism_app/lib/

	# Visual C++ Runtime DLLs
	cp "C:\Program Files\Graphviz\bin\vcruntime140.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\vcruntime140_1.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140_1.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140_2.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140_atomic_wait.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\msvcp140_codecvt_ids.dll" prism_app/lib/
	cp "C:\Program Files\Graphviz\bin\concrt140.dll" prism_app/lib/

	# MinGW runtime DLLs (already included)
	cp "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll" prism_app/lib/
	cp "C:\msys64\mingw64\bin\libstdc++-6.dll" prism_app/lib/
	cp "C:\msys64\mingw64\bin\libwinpthread-1.dll" prism_app/lib/

	# GraphViz configuration
	cp "C:\Program Files\Graphviz\bin\config6" prism_app/lib/
