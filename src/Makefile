TARGET   := ./symac
#CXXFLAGS := -g -Wextra -Wall -Wno-long-long -pedantic-errors -Werror -Wfatal-errors -std=c++17
CXXFLAGS := -g -Wextra -Wall -pedantic-errors -std=c++17
CXX      := g++
INCLUDES := 
LIBS     := -lginac -lcln -lboost_program_options
EXT      := cpp
BUILDDIR := .build

override BUILDDIR := $(strip $(BUILDDIR))
SOURCES  := $(wildcard *.$(EXT))
SOURCES  += $(wildcard parser/*.$(EXT))
SOURCES  += $(wildcard transfer_function/*.$(EXT))
SOURCES  += $(wildcard simplification/*.$(EXT))
SOURCES  += $(wildcard solver/*.$(EXT))
SOURCES  += $(wildcard export/*.$(EXT))
OBJECTS  := $(patsubst %.$(EXT), $(BUILDDIR)/%.o, $(SOURCES))
DEPS     := $(patsubst %.$(EXT), $(BUILDDIR)/%.dep, $(SOURCES))

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS) $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LIBS)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

$(OBJECTS): $(BUILDDIR)/%.o: %.$(EXT) $(BUILDDIR)/%.dep $(BUILDDIR)/.tag
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(DEPS): $(BUILDDIR)/%.dep: %.$(EXT) $(BUILDDIR)/.tag
	mkdir -p $(dir $(@))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $< -MT $@ -MT $(<:.$(EXT)=.o) -o $@

%.tag:
	mkdir -p $(dir $(@))
	touch $@

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)
