CXX      := g++
CXXFLAGS := -ggdb -std=c++17
CPPFLAGS := -MMD

COMPILE  := $(CXX) $(CXXFLAGS) $(CPPFLAGS)

SRCS     := ast_printer_driver.cpp generate_ast.cpp prism.cpp
DEPS     := $(SRCS:.cpp=.d)


prism: expr.h stmt.h prism.o
	@$(COMPILE) prism.o -o $@


ast_printer: expr.h stmt.h ast_printer_driver.o
	@$(COMPILE) ast_printer_driver.o -o $@


generate_ast: generate_ast.o
	@$(COMPILE) $< -o $@


expr.h stmt.h: generate_ast
	@./generate_ast .


.PHONY: clean
clean:
	rm -f *.d *.o ast_printer generate_ast prism


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
