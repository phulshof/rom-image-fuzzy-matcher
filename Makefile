TARGET = RomImageFuzzyMatcher


#---------------------------------------------------------------------------------
#  Icon and picture for your game in menu
#---------------------------------------------------------------------------------
#   Uncomment and change to your picture name
#PSP_EBOOT_PIC1 = PIC1.PNG
#PSP_EBOOT_ICON = ICON0.PNG

#---------------------------------------------------------------------------------
#  Account for normal c/cpp files
#---------------------------------------------------------------------------------
CFILES := $(wildcard *.c)
CPPFILES := $(wildcard *.cpp)

export OBJS += \
       $(CFILES:.c=.o) \
       $(CPPFILES:.cpp=.o) \

#---------------------------------------------------------------------------------
## Any other build targets you want to add
#---------------------------------------------------------------------------------
#OBJS += logo.o

CDEPS = $(CFILES:.c=.d)
CPPDEPS = $(CPPFILES:.cpp=.d)
DEPS = $(CDEPS) $(CPPDEPS)

SOURCES = $(CFILES) $(CPPFILES)

INCDIR =  ./ /usr/include 

CFLAGS = -g3 -O0 -Wall 
CFLAGS += $(addprefix -I,$(INCDIR)) $(RELEASE_FLAGS) 

CXXFLAGS = $(CFLAGS) 

#  Had to allow rtti for boost library...
#-fno-rtti

ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lc -lstdc++ \
			
#	-lmikmod -lpspaudio -lpspgum -lpspgu -lpsprtc  \
#	-lpng -lz -lm -lpsppower \
#	-lpspmp3 \
#	-lpspwlan \
#	-lpspusb \
#	-lpspusbstor \
#	-lpspgum \
#	-lpspgu \
#	-lm \
#	-lpspumd \
#	-lpspnet_adhocctl \
#	-lpspnet_adhocmatching \
#	-lpspnet_adhoc \


# Main target
$(TARGET): $(OBJS)

#---------------------------------------------------------------------------------
#   Release details
#---------------------------------------------------------------------------------
REL_OPTIMIZE = -O2
export RELEASE_FOLDER = release_dir/TimeCube/

export RELEASE_FLAGS = -DBUILD_TYPE=RELEASE -DRELEASE $(REL_OPTIMIZE)

#  Will create the release in RELEASE_FOLDER and ready for transfer to
#    PSP
release: ctags
	$(MAKE) "CFLAGS= $(addprefix -I,$(INCDIR)) $(RELEASE_FLAGS) \
				-G0 -Wall -DPSP_BUILD "
	-mkdir -p $(RELEASE_FOLDER)
	cp -v EBOOT.PBP $(RELEASE_FOLDER)/
	$(MAKE) release -C Music
	$(MAKE) release -C Sounds
	$(MAKE) release -C Images

rar: ctags
	rar a $(TARGET).rar $(RELEASE_FOLDER)

#---------------------------------------------------------------------------------
#    Remake editor tags
#---------------------------------------------------------------------------------
tags: $(SOURCES)
	-ctags -R --sort=yes --c++-kinds=+cdefgmnpstux \
	  --fields=+iaKS --extra=+q ./ 

ctags: tags

sdk_ctags:
	-ctags -R -o $(HOME)/.vim/ctags/psp_cpp --sort=yes --c++-kinds=+cdefgmnpstux \
	  --fields=+iaKS --extra=+q $(INCDIR) $(PSPSDK) $(PSPDIR)

#---------------------------------------------------------------------------------
# Rules for building cpp files (if you have them)
#---------------------------------------------------------------------------------
%.o: %.cpp 
	@echo $(notdir $<)
	$(MAKE) tags
	$(CXX) -MMD -MP -MF $*.d $(CXXFLAGS) -c $< -o $@

#---------------------------------------------------------------------------------
#  Rules for building c files if you have them
#---------------------------------------------------------------------------------
%.o:  %.c 
	@echo $(notdir $<)
	$(MAKE) tags
	$(CC) -MMD -MP -MF $*.d $(CFLAGS) -c $< -o $@

-include $(DEPS)


##---------------------------------------------------------------------------------
##   Maybe for a log target, you could use something like this
##---------------------------------------------------------------------------------
#logo.o: logo.raw
#	bin2o -i logo.raw logo.o logo

.IGNORE: clean

#---------------------------------------------------------------------------------
#   Get rid of all the intermediary makefiles (.d files)
#---------------------------------------------------------------------------------
clean: 
	rm $(OBJS) $(TARGET)

clean-deps:
	-rm $(DEPS)

#---------------------------------------------------------------------------------
#   Clean Deps and all object files
#---------------------------------------------------------------------------------
clean-all: clean-deps clean

diff:
	svn diff --diff-cmd=diffwrap

#-----------------------------------------------------------
# Ctags flag info:
#-----------------------------------------------------------
#C++
#    c  classes
#    d  macro definitions
#    e  enumerators (values inside an enumeration)
#    f  function definitions
#    g  enumeration names
#    l  local variables [off]
#    m  class, struct, and union members
#    n  namespaces
#    p  function prototypes [off]
#    s  structure names
#    t  typedefs
#    u  union names
#    v  variable definitions
#    x  external and forward variable declarations [off]
#
