all: kddbeta.out

kddbeta.out : main.c IOutilities.c
	gcc -o kddbeta.out main.c savefile.c IOutilities.c coord.c gameAction.c gameBattle.c gameData.c gameNewDay.c -lm

clean:
	rm kddbeta.out
