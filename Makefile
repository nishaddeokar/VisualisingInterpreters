CXX      := g++
CXXFLAGS := -ggdb -std=c++17
CPPFLAGS := -MMD

# Apple Silicon Mac GraphViz directories (Homebrew)
GRAPHVIZ_INCLUDE := -I/opt/homebrew/include
GRAPHVIZ_LIBS    := -L/opt/homebrew/lib -lgvc -lcgraph

# Add GraphViz include path to compilation flags
CPPFLAGS += $(GRAPHVIZ_INCLUDE)

COMPILE  := $(CXX) $(CXXFLAGS) $(CPPFLAGS)

SRCS     := ast_printer_driver.cpp prism.cpp
DEPS     := $(SRCS:.cpp=.d)


prism: expr.h stmt.h prism.o
	@$(COMPILE) prism.o -o $@ $(GRAPHVIZ_LIBS)


ast_printer: expr.h stmt.h ast_printer_driver.o
	@$(COMPILE) ast_printer_driver.o -o $@ $(GRAPHVIZ_LIBS)


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
