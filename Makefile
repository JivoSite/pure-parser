# ---- Environment ----

COMPILE=clang++ -std=c++17
ARCHIVE=ar rs
DIR=build

# ---- Public ----

cpp_lib: cpp_compile
	make dir_clean

cpp_run: PureParserExamples
	$(DIR)/PureParserExamples
	make dir_clean

c_lib: c_compile
	make dir_clean

c_run: pure_parser_examples
	$(DIR)/pure_parser_examples
	make dir_clean

swift_run:
	swift run

verify:
	make dir_clean
	make cpp_lib
	test -e $(DIR)/libPureParser.a
	test -e $(DIR)/PureParser.hpp
	test -e $(DIR)/PureElement.hpp
	make dir_clean

	make dir_clean
	make c_lib
	test -e $(DIR)/libpureparser.a
	test -e $(DIR)/pure_parser.h
	make dir_clean

	make dir_clean
	make cpp_run
	make dir_clean

	make dir_clean
	make c_run
	make dir_clean

# ---- Private ----

dir_create:
	mkdir -p $(DIR)

dir_clean:
	rm -f $(DIR)/*.o

cpp_compile: libPureParser.a
	cp cpp_src/PureElement.hpp cpp_src/PureParser.hpp $(DIR)

PureScanner.o: dir_create
	$(COMPILE) -o $(DIR)/PureScanner.o -c cpp_src/PureScanner.cpp

PureParser.o: dir_create
	$(COMPILE) -o $(DIR)/PureParser.o -c cpp_src/PureParser.cpp

libPureParser.a: PureScanner.o PureParser.o
	$(ARCHIVE) $(DIR)/libPureParser.a $(DIR)/PureScanner.o $(DIR)/PureParser.o

PureParserExamples: libPureParser.a
	$(COMPILE) -o $(DIR)/PureParserExamples cpp_src/PureParserExamples.cpp -L$(DIR) -lPureParser

c_compile: libpureparser.a
	cp c_wrapper/pure_parser.h $(DIR)

pure_parser.o:
	$(COMPILE) -o $(DIR)/pure_parser.o -c c_wrapper/pure_parser.cpp

libpureparser.a: PureScanner.o PureParser.o pure_parser.o
	$(ARCHIVE) $(DIR)/libpureparser.a $(DIR)/PureScanner.o $(DIR)/PureParser.o $(DIR)/pure_parser.o

pure_parser_examples: libpureparser.a
	$(COMPILE) -o $(DIR)/pure_parser_examples c_wrapper/pure_parser_examples.c -L$(DIR) -lpureparser
