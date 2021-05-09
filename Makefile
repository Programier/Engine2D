CC		= g++
FLAGS		= -Wall -fPIC -c
LFLAGS		= -lglfw -lpng -lGL -lGLEW -lalut -lopenal -laudio -ldl
OUTDIR		= Release
DONE		= @echo "DONE"
NAME		= FlappyBird



all: clean_all engine game
	
game: clean
	@echo "Compiling the game"
	@$(CC) -Wall game.cpp Engine2D/*.cpp -o $(NAME) $(LFLAGS)
	@mkdir -p $(OUTDIR)
	@cp -r images ./$(OUTDIR)
	@cp -r audio ./$(OUTDIR)
	@mv $(NAME) ./$(OUTDIR)
	$(DONE)
engine:
	@mkdir -p $(OUTDIR)
	@rm -rf $(OUTDIR)/lib
	@cd Engine2D && $(MAKE)
	@mv Engine2D/lib ./$(OUTDIR)
clean:
	@rm -rf $(OUTDIR)/audio
	@rm -rf $(OUTDIR)/images
	@rm -rf $(OUTDIR)/$(NAME)
	
clean_all: 
	@cd Engine2D && $(MAKE) clean
	@rm -rf $(OUTDIR)
