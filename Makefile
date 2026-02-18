# SEAL 2.0 Makefile
# Written by Owen Rudge

CFLAGS   = -s -Iinclude
CPPFLAGS = -s -Iinclude

OBJ_DIR = obj

OBJ_LIST = \
$(OBJ_DIR)/dlxload.o \
$(OBJ_DIR)/data/registry.o \
$(OBJ_DIR)/data/dataini.o \
$(OBJ_DIR)/object/view.o \
$(OBJ_DIR)/controls/button.o \
$(OBJ_DIR)/controls/app.o \
$(OBJ_DIR)/controls/inputbox.o \
$(OBJ_DIR)/controls/menus.o \
$(OBJ_DIR)/controls/trackbar.o \
$(OBJ_DIR)/controls/treeview.o \
$(OBJ_DIR)/controls/checkbox.o \
$(OBJ_DIR)/controls/iodlg.o \
$(OBJ_DIR)/controls/dialogs.o \
$(OBJ_DIR)/controls/editor.o \
$(OBJ_DIR)/controls/scroller.o \
$(OBJ_DIR)/controls/stattext.o \
$(OBJ_DIR)/controls/tabbook.o \
$(OBJ_DIR)/engine.o \
$(OBJ_DIR)/compat.o \
$(OBJ_DIR)/safmem.o \
$(OBJ_DIR)/text.o \
$(OBJ_DIR)/object/filter.o \
$(OBJ_DIR)/object/rect.o \
$(OBJ_DIR)/object/object.o \
$(OBJ_DIR)/grfx-f.o \
$(OBJ_DIR)/txts.o \
$(OBJ_DIR)/colors.o \
$(OBJ_DIR)/bmps.o \
$(OBJ_DIR)/skin.o \
$(OBJ_DIR)/drivers.o \
$(OBJ_DIR)/drivers/keyboard.o \
$(OBJ_DIR)/drivers/mouse.o \
$(OBJ_DIR)/drivers/savescr.o \
$(OBJ_DIR)/drivers/sound.o \
$(OBJ_DIR)/screen.o \
$(OBJ_DIR)/list.o \
$(OBJ_DIR)/lnk.o \
$(OBJ_DIR)/files.o \
$(OBJ_DIR)/vfile.o \
$(OBJ_DIR)/ico.o \
$(OBJ_DIR)/stimer.o \
$(OBJ_DIR)/controls/grfx.o \
$(OBJ_DIR)/controls/window.o \
$(OBJ_DIR)/tips.o \
$(OBJ_DIR)/xdldata.o

LIB_LIST = \
lib/libmss.a \
lib/libalttf.a \
lib/gbm.a \
lib/libjpeg.a \
lib/libldpng.a \
lib/libpng.a \
lib/libz.a \
lib/libaastr.a

XDL_LIST = \
bin/libs/badctrls.xdl \
bin/libs/sfamod.xdl \
bin/libs/sfamp3.xdl \
bin/libs/sfamidi.xdl \
bin/libs/sfawav.xdl \
#bin/libs/sfaogg.xdl \
bin/libs/helpsys.xdl \
bin/libs/cp.xdl \
bin/libs/ds.xdl \
bin/libs/ss.xdl \
bin/libs/effect.xdl \
bin/libs/volctrl.xdl \
bin/fire.sss \
bin/black.sss \
bin/libs/as.xdl \
bin/libs/ssc.xdl \
bin/libs/m.xdl \
bin/libs/cards.xdl \
bin/libs/print.xdl \
bin/libs/sscript.xdl

S2A_LIST = \
bin/apps/seal/calc.s2a \
bin/apps/seal/cdplayer.s2a \
bin/apps/seal/s2about.s2a \
bin/apps/seal/dc.s2a \
bin/apps/seal/explorer.s2a \
bin/apps/seal/help.s2a \
bin/apps/seal/imager.s2a \
bin/apps/seal/player.s2a \
bin/apps/seal/run.s2a \
bin/apps/seal/smalldit.s2a \
bin/apps/seal/sregedit.s2a \
bin/sealdos.s2a \
bin/desktop.s2a \
bin/apps/seal/fifteen.s2a \
bin/apps/seal/snake.s2a \
bin/apps/seal/sol.s2a \
bin/apps/seal/unis.s2a \
bin/apps/seal/towofhan.s2a \
bin/apps/seal/klondike.s2a \
bin/apps/seal/cruel.s2a \
bin/apps/seal/golf.s2a \
bin/apps/seal/dskimage.s2a \
bin/apps/seal/ssdemo.s2a

all : $(OBJ_LIST) dlxgen.exe dlxmake.exe dlxinsd.exe bin/seal.exe bin/regedit.exe setup/install.exe $(XDL_LIST) $(S2A_LIST)
	@echo SEAL has been built.

$(OBJ_DIR)/%.o: src/%.c
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/%.cpp
	gcc $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/%.cc
	gcc $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/libs/%.o: src/libs/%.c
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/libs/sfa/%.o: src/libs/sfa/%.c
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/libs/sscript/%.o: src/libs/sscript/%.c
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/apps/%.o: src/apps/%.c
	gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/setup/%.o: src/install/%.c
	gcc $(CFLAGS) -Os -c $< -o $@

dlxgen.exe : obj/dlxgen.o
#	gcc $(CFLAGS) -o obj/dlxgen.o -c src/dlxgen.c
	gcc $(CFLAGS) -DNO_SEAL_HEADERS -o obj/dgdatini.o -c src/data/dataini.c
	gcc -o dlxgen.exe obj/dlxgen.o obj/dgdatini.o lib/libz.a

dlxmake.exe : 
	gcc $(CPPFLAGS) -o obj/dlxmake.o -c src/dlxmake.cc
	gcc -o dlxmake.exe obj/dlxmake.o

dlxinsd.exe : 
	gcc $(CFLAGS) -o obj/dlxinsd.o -c src/dlxinsd.c
	gcc -o dlxinsd.exe obj/dlxinsd.o

bin/seal.exe : $(OBJ_LIST)
	gcc -o bin\seal.exe $(OBJ_LIST) $(LIB_LIST) -lalleg
	exe2coff bin\seal.exe
	copy /B CWSDSTUB.EXE+bin\seal bin\seal.exe
	del bin\seal

bin/regedit.exe :
	gcc $(CFLAGS) -c src/data/regedit.c -o obj/regedit.o
	gcc -o bin/regedit.exe obj/regedit.o

bin/libs/badctrls.xdl : $(OBJ_DIR)/libs/badctrls.o
	dlxgen -Cbin/libs/badctrls.xdl obj/libs/badctrls.o
	dlxinsd bin/libs/badctrls.xdl src/libs/badctrls.dat

bin/apps/seal/calc.s2a : $(OBJ_DIR)/apps/calc.o
	dlxgen -Cbin/apps/seal/calc.s2a -Rsrc/apps/calc.srs obj/apps/calc.o

bin/apps/seal/cdplayer.s2a : $(OBJ_DIR)/apps/cdplayer.o
#	gcc $(CFLAGS) -fomit-frame-pointer -mpentium -funroll-loops -ffast-math -O3 -Wall -c src/apps/cdplayer.c -o obj/apps/cdplayer.o
	dlxgen -Cbin/apps/seal/cdplayer.s2a -Rsrc/apps/cdplayer.srs obj/apps/cdplayer.o lib/libbcd.a

bin/libs/sfamod.xdl : $(OBJ_DIR)/libs/sfa/sfamod.o
	dlxgen -Cbin/libs/sfamod.xdl -Rsrc/libs/sfa/sfamod.exp obj/libs/sfa/sfamod.o lib/libjgmod.a

bin/libs/sfamp3.xdl : $(OBJ_DIR)/libs/sfa/sfamp3.o
	dlxgen -Cbin/libs/sfamp3.xdl -Rsrc/libs/sfa/sfamp3.exp obj/libs/sfa/sfamp3.o lib/libamp.a

bin/libs/sfamidi.xdl : $(OBJ_DIR)/libs/sfa/sfamidi.o
	dlxgen -Cbin/libs/sfamidi.xdl -Rsrc/libs/sfa/sfamidi.exp obj/libs/sfa/sfamidi.o

bin/libs/sfawav.xdl : $(OBJ_DIR)/libs/sfa/sfawav.o
	dlxgen -Cbin/libs/sfawav.xdl -Rsrc/libs/sfa/sfawav.exp obj/libs/sfa/sfawav.o

#bin/libs/sfaogg.xdl : $(OBJ_DIR)/libs/sfa/sfaogg.o
#  dlxgen -Cbin/libs/sfaogg.xdl -Rsrc/libs/sfa/sfaogg.exp obj/libs/sfa/sfaogg.o lib/allegogg.a lib/vorbisfil.a lib/vorbis.a lib/ogg.a

bin/apps/seal/s2about.s2a : $(OBJ_DIR)/apps/s2about.o
	dlxgen -Cbin/apps/seal/s2about.s2a -Rsrc/apps/s2about.srs obj/apps/s2about.o

bin/apps/seal/dc.s2a : $(OBJ_DIR)/apps/dc.o
	dlxgen -Cbin/apps/seal/dc.s2a -Rsrc/apps/dc.srs obj/apps/dc.o

bin/apps/seal/explorer.s2a : $(OBJ_DIR)/apps/explorer.o
	dlxgen -Cbin/apps/seal/explorer.s2a -Rsrc/apps/explorer.srs obj/apps/explorer.o

bin/apps/seal/help.s2a : $(OBJ_DIR)/apps/help.o
	dlxgen -Cbin/apps/seal/help.s2a -Rsrc/apps/help.srs obj/apps/help.o

bin/apps/seal/imager.s2a : $(OBJ_DIR)/apps/imager.o
	dlxgen -Cbin/apps/seal/imager.s2a -Rsrc/apps/imager.srs obj/apps/imager.o

bin/apps/seal/player.s2a : $(OBJ_DIR)/apps/player.o
	dlxgen -Cbin/apps/seal/player.s2a -Rsrc/apps/player.srs obj/apps/player.o

bin/apps/seal/run.s2a : $(OBJ_DIR)/apps/run.o
	dlxgen -Cbin/apps/seal/run.s2a -Rsrc/apps/run.srs obj/apps/run.o

bin/sealdos.s2a : $(OBJ_DIR)/sealdos.o
	dlxgen -Cbin/sealdos.s2a -Rsrc/sealdos.srs obj/sealdos.o

bin/apps/seal/smalldit.s2a : $(OBJ_DIR)/apps/smalldit.o
	dlxgen -Cbin/apps/seal/smalldit.s2a -Rsrc/apps/smalldit.srs obj/apps/smalldit.o

bin/apps/seal/sregedit.s2a : $(OBJ_DIR)/apps/sregedit.o
	dlxgen -Cbin/apps/seal/sregedit.s2a -Rsrc/apps/sregedit.srs obj/apps/sregedit.o

bin/libs/helpsys.xdl : $(OBJ_DIR)/apps/helpsys.o
	dlxgen -Cbin/libs/helpsys.xdl -Rsrc/apps/helpsys.srs obj/apps/helpsys.o

bin/desktop.s2a : $(OBJ_DIR)/desktop/desktop.o $(OBJ_DIR)/desktop/dclock.o $(OBJ_DIR)/desktop/fileext.o $(OBJ_DIR)/desktop/dicon.o $(OBJ_DIR)/desktop/diconman.o $(OBJ_DIR)/desktop/free.o
	dlxgen -Cbin/desktop.s2a -Rsrc/desktop/desktop.srs $(OBJ_DIR)/desktop/desktop.o $(OBJ_DIR)/desktop/dclock.o $(OBJ_DIR)/desktop/fileext.o  $(OBJ_DIR)/desktop/dicon.o $(OBJ_DIR)/desktop/diconman.o $(OBJ_DIR)/desktop/free.o

bin/libs/cp.xdl : $(OBJ_DIR)/libs/cp.o
	dlxgen -Cbin/libs/cp.xdl obj/libs/cp.o

bin/libs/ds.xdl : $(OBJ_DIR)/libs/ds.o
	dlxgen -Cbin/libs/ds.xdl obj/libs/ds.o

bin/libs/ss.xdl : $(OBJ_DIR)/libs/ss.o
	dlxgen -Cbin/libs/ss.xdl obj/libs/ss.o

bin/libs/as.xdl : $(OBJ_DIR)/libs/as.o
	dlxgen -Cbin/libs/as.xdl obj/libs/as.o

bin/libs/ssc.xdl : $(OBJ_DIR)/libs/ssc.o
	dlxgen -Cbin/libs/ssc.xdl obj/libs/ssc.o

bin/libs/m.xdl : $(OBJ_DIR)/libs/m.o
	dlxgen -Cbin/libs/m.xdl obj/libs/m.o

bin/libs/effect.xdl : $(OBJ_DIR)/libs/effect.o
	dlxgen -Cbin/libs/effect.xdl obj/libs/effect.o

bin/libs/volctrl.xdl : $(OBJ_DIR)/libs/volctrl.o
	dlxgen -Cbin/libs/volctrl.xdl obj/libs/volctrl.o

bin/fire.sss : $(OBJ_DIR)/sss/fire.o
	dlxgen -Cbin/fire.sss obj/sss/fire.o

bin/black.sss : $(OBJ_DIR)/sss/black.o
	dlxgen -Cbin/black.sss obj/sss/black.o

bin/libs/cards.xdl : $(OBJ_DIR)/libs/cards.o
	dlxgen -Cbin/libs/cards.xdl obj/libs/cards.o
	dlxinsd bin/libs/cards.xdl src/libs/cards.dat

bin/libs/print.xdl : $(OBJ_DIR)/libs/print.o
	dlxgen -Cbin/libs/print.xdl obj/libs/print.o

bin/libs/sscript.xdl : $(OBJ_DIR)/libs/sscript/sscript.o
	dlxgen -Cbin/libs/sscript.xdl obj/libs/sscript/sscript.o

bin/apps/seal/fifteen.s2a : $(OBJ_DIR)/apps/games/fifteen.o
	dlxgen -Cbin/apps/seal/fifteen.s2a obj/apps/games/fifteen.o

bin/apps/seal/snake.s2a : $(OBJ_DIR)/apps/games/snake.o
	dlxgen -Cbin/apps/seal/snake.s2a obj/apps/games/snake.o
	dlxinsd bin/apps/seal/snake.s2a src/apps/games/snake.dat

bin/apps/seal/sol.s2a : $(OBJ_DIR)/apps/games/sol.o
	dlxgen -Cbin/apps/seal/sol.s2a obj/apps/games/sol.o

bin/apps/seal/unis.s2a : $(OBJ_DIR)/apps/games/unis.o
	dlxgen -Cbin/apps/seal/unis.s2a obj/apps/games/unis.o

bin/apps/seal/towofhan.s2a : $(OBJ_DIR)/apps/games/towofhan.o
	dlxgen -Cbin/apps/seal/towofhan.s2a obj/apps/games/towofhan.o

bin/apps/seal/klondike.s2a : $(OBJ_DIR)/apps/games/klondike.o
	dlxgen -Cbin/apps/seal/klondike.s2a obj/apps/games/klondike.o

bin/apps/seal/cruel.s2a : $(OBJ_DIR)/apps/games/cruel.o
	dlxgen -Cbin/apps/seal/cruel.s2a obj/apps/games/cruel.o

bin/apps/seal/golf.s2a : $(OBJ_DIR)/apps/games/golf.o
	dlxgen -Cbin/apps/seal/golf.s2a obj/apps/games/golf.o

bin/apps/seal/dskimage.s2a : $(OBJ_DIR)/apps/dskimage.o
	dlxgen -Cbin/apps/seal/dskimage.s2a obj/apps/dskimage.o

bin/apps/seal/ssdemo.s2a : $(OBJ_DIR)/apps/ssdemo.o
	dlxgen -Cbin/apps/seal/ssdemo.s2a obj/apps/ssdemo.o

setup/install.exe : $(OBJ_DIR)/setup/cmdparse.o $(OBJ_DIR)/setup/install.o $(OBJ_DIR)/setup/scrpages.o
	gcc -Os -o setup/install.exe $(OBJ_DIR)/setup/cmdparse.o $(OBJ_DIR)/setup/install.o $(OBJ_DIR)/setup/scrpages.o
	strip setup/install.exe
	exe2coff setup/install.exe
	copy /B CWSDSTUB.EXE+setup\install setup\install.exe
	del setup\install

clean :
	del $(subst /,\,$(OBJ_DIR)/libs/*.o)
	del $(subst /,\,$(OBJ_DIR)/libs/sfa/*.o)
	del $(subst /,\,$(OBJ_DIR)/libs/sscript/*.o)
	del $(subst /,\,$(OBJ_DIR)/apps/games/*.o)
	del $(subst /,\,$(OBJ_DIR)/apps/*.o)
	del $(subst /,\,$(OBJ_DIR)/object/*.o)
	del $(subst /,\,$(OBJ_DIR)/controls/*.o)
	del $(subst /,\,$(OBJ_DIR)/drivers/*.o)
	del $(subst /,\,$(OBJ_DIR)/setup/*.o)
	del $(subst /,\,$(OBJ_DIR)/data/*.o)
	del $(subst /,\,$(OBJ_DIR)/sss/*.o)
	del $(subst /,\,$(OBJ_DIR)/*.o)
	del setup\install.exe
	del bin\regedit.exe
	del bin\seal.exe
	del dlxgen.exe
	del dlxinsd.exe
	del dlxmake.exe
	del bin\seal.dbg
	del bin\mss.log

clean-xdl :
	del $(subst /,\,bin/apps/seal/*.s2a)
	del $(subst /,\,bin/libs/*.xdl)
	del $(subst /,\,bin/*.s2a)
	del $(subst /,\,bin/*.xdl)
	del $(subst /,\,bin/*.sss)

compress :
	upx -9 bin/seal.exe bin/regedit.exe dlxgen.exe dlxinsd.exe dlxmake.exe setup/install.exe

registry :
	cd bin
	regedit import.reg > NUL
	cd ..
