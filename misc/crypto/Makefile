.PHONY : all
all: ${patsubst %.c, %.o, ${wildcard *.c}}

#
# remove object files
#

.PHONY : tidy
tidy: clean

.PHONY : clean
clean:
	@${RM} *.o
