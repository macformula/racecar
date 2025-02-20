CTRL_SRC = $(wildcard src/*.cc)
CTRL_OBJ = $(patsubst %.cc,%.o,$(CTRL_SRC))

TEST_SRC = $(wildcard test/*.cc)

INC = -I. -I ../ -I ../../../

FLAGS = $(INC) -std=c++20

test: $(CTRL_OBJ) $(TEST_SRC)
	g++ $^ -o $@ $(FLAGS)

$(CTRL_OBJ): %.o: %.cc
	g++ -c $^ -o $@ $(FLAGS)

.PHONY: clean
clean:
	rm -rf **/*.o *.exe