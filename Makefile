main:
	g++ -std=c++0x -o tokenizer main.cpp Tokenizer.cpp Parser.cpp -lm -O3

clean:
	rm -rf a.out
