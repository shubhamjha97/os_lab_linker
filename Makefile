
main:
	g++ -std=c++0x -o tokenizer main.cpp Tokenizer.cpp Parser.cpp

clean:
	rm -rf a.out
