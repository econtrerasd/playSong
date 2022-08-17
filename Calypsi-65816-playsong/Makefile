VPATH = src

FOENIX = module/Calypsi-65816-Foenix

# Common source files
ASM_SRCS =
C_SRCS = main.c playsong.c

MODEL = --code-model=large --data-model=small
LIB_MODEL = lc-sd

FOENIX_LIB = $(FOENIX)/Foenix-$(LIB_MODEL).a
FOENIX_LINKER_RULES = $(FOENIX)/linker-files/Foenix-U+.scm

# Object files
OBJS = $(ASM_SRCS:%.s=obj/%.o) $(C_SRCS:%.c=obj/%.o)
OBJS_DEBUG = $(ASM_SRCS:%.s=obj/%-debug.o) $(C_SRCS:%.c=obj/%-debug.o)

obj/%.o: %.s
	as65816 --core=65816 $(MODEL) --target=Foenix --speed --list-file=$(@:%.o=%.lst) -o $@ $<

obj/%.o: %.c
	cc65816 --core=65816 $(MODEL) --target=Foenix -I ./include --speed -O2 --debug --list-file=$(@:%.o=%.lst) -o $@ $<

obj/%-debug.o: %.s
	as65816 --core=65816 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

obj/%-debug.o: %.c
	cc65816 --core=65816 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -o $@ $<

play.elf: $(OBJS_DEBUG)
	#ln65816 --debug -o $@ $^ $(FOENIX_LINKER_RULES) clib-$(LIB_MODEL)-Foenix.a --list-file=spredit-debug.lst --cross-reference --rtattr printf=reduced --semi-hosted --copy-initialize palette0 
	ln65816  --list-file=play-debug.lst --cross-reference --rtattr printf=reduced ---semi-hosted -rtattr cstartup=Foenix --copy-initialize palette0 -o $@ $^ linker-large-small.scm clib-$(LIB_MODEL)-Foenix.a 

play.pgz:  $(OBJS) $(FOENIX_LIB)
	ln65816 --output-format=pgz --list-file=play-Foenix.lst --cross-reference --rtattr printf=float --rtattr cstartup=Foenix --verbose --copy-initialize palette0 -o $@ $^ $(FOENIX_LINKER_RULES) clib-$(LIB_MODEL)-Foenix.a 

$(FOENIX_LIB):
	(cd $(FOENIX) && make all)

clean:
	-rm $(OBJS) $(OBJS:%.o=%.lst) $(OBJS_DEBUG) $(OBJS_DEBUG:%.o=%.lst) $(FOENIX_LIB)
	-rm play.elf play.pgz play-debug.lst play-Foenix.lst
	-(cd $(FOENIX) && make clean)
