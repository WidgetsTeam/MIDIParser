lib/libmidiparser.a: obj/File.o obj/Track.o obj/Event.o obj/Error.o
	ar rcs $@ $^

obj/%.o: src/%.cpp
	g++ -c -o $@ $< -Iinclude/ -std=c++11

clean:
	rm obj/*

install:
	cp -R include/MidiParser /usr/include
	cp lib/libmidiparser.a /usr/lib 

