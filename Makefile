
main:
	g++ -std=c++0x -o linker main.cpp Tokenizer.cpp Parser.cpp

clean:
	rm -rf linker
