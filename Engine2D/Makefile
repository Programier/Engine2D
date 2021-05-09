CC		= g++
FLAGS		= -Wall -fPIC -c
SFLAGS		= -shared -o
LFLAGS		= -lglfw -lpng -lGL -lGLEW -lalut -lopenal -laudio
OUTDIR		= lib
DONE		= @echo "DONE"

WDIR		= Window
WOBJS		= Window.o
WSOURCE		= Window.cpp
WNAME		= Window.lib

# Event
EDIR		= Event
EOBJS		= Event.o
ESOURCE		= Event.cpp
ENAME		= Event.lib

# Music
MDIR		= Music
MOBJS		= Music.o
MSOURCE		= Music.cpp
MNAME		= Music.lib

# Graphics
GDIR		= Graphics
GOBJS		= Button.o Graphics.o Shader.o Texture.o Texture_Loader.o Texture_Renderer.o
GSOURCE		= Button.cpp Graphics.cpp Shader.cpp Texture.cpp Texture_Loader.cpp Texture_Renderer.cpp
GNAME		= Graphics.lib


all: clean 
	@mkdir $(OUTDIR)
	@echo "Compiling the engine"
	@make window_lib
	@make event_lib
	@make music_lib
	@make graphics_lib
	@mv $(WDIR)/$(WNAME) $(OUTDIR)
	@mv $(EDIR)/$(ENAME) $(OUTDIR)
	@mv $(MDIR)/$(MNAME) $(OUTDIR)
	@mv $(GDIR)/$(GNAME) $(OUTDIR)
	@echo "Engine compilation completed"
	
	
# Window 
window_lib: window_obj	
	@echo "Start compiling the $(WNAME) library"
	@cd $(WDIR) && $(CC) $(SFLAGS) $(WNAME) $(WOBJS)  $(LFLAGS)
	$(DONE)

window_obj:
	@echo "Start compiling the $(WNAME) objects"
	@cd $(WDIR) && $(CC) $(FLAGS) $(WSOURCE) $(LFLAGS)

window_clean:
	@cd $(WDIR) && rm -f $(WOBJS) $(WNAME)
	
	
	
# Event
event_lib: event_obj	
	@echo "Start compiling the $(ENAME) library"
	@cd $(EDIR) && $(CC) $(SFLAGS) $(ENAME) $(EOBJS)  $(LFLAGS)
	$(DONE)
	
event_obj:
	@echo "Start compiling the $(ENAME) objects"
	@cd $(EDIR) && $(CC) $(FLAGS) $(ESOURCE) $(LFLAGS)
	
event_clean:
	@cd $(EDIR) && rm -f $(EOBJS) $(ENAME)

	
	
# Music
music_lib: music_obj	
	@echo "Start compiling the $(MNAME) library"
	@cd $(MDIR) && $(CC) $(SFLAGS) $(MNAME) $(MOBJS)  $(LFLAGS)
	$(DONE)
	
music_obj:
	@echo "Start compiling the $(MNAME) objects"
	@cd $(MDIR) && $(CC) $(FLAGS) $(MSOURCE) $(LFLAGS)
	
music_clean:
	@cd $(MDIR) && rm -f $(MOBJS) $(MNAME)
	

	
	
# Graphics
graphics_lib: graphics_obj
	@echo "Start compiling the $(GNAME) library"
	@cd $(GDIR) && $(CC) $(SFLAGS) $(GNAME) $(GOBJS)  $(LFLAGS)
	$(DONE)
	
graphics_obj:
	@echo "Start compiling the $(GNAME) objects"
	@cd $(GDIR) && $(CC) $(FLAGS) $(GSOURCE) $(LFLAGS)
	
graphics_clean:
	@cd $(GDIR) && rm -f $(GOBJS) $(GNAME)
	
	
	
clean: window_clean event_clean music_clean graphics_clean
	@rm -rf $(OUTDIR)
