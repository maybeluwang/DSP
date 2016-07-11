# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
SRIPATH ?= $(shell pwd)/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map


.PHONY: all clean map run

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
		$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
		$(CXX) $(CXXFLAGS) -c $<

Result1:
	@echo "Check directory testdata_sep";
	@if [ ! -d testdata_sep ]; then \
		mkdir testdata_sep; \
	fi;
	for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt"; \
		perl ./separator_big5.pl testdata/$$i.txt > testdata_sep/$$i.txt; \
	$(SRIPATH)/bin/i686-m64/disambig -text testdata_sep/$$i.txt -map $(TO) -lm $(LM) -order 2 > result1/$$i.txt; \
	done;

run:
	@echo "Check directory result2";
	@if [ ! -d result2 ]; then \
		mkdir result2; \
	fi;
	@for i in $(shell seq 1 10) ; do \
		echo "Running $$i.txt"; \
		./mydisambig -text testdata/$$i.txt -map $(TO) -lm $(LM) -order 2 > result2/$$i.txt; \
	done;

map:
	@echo "Compile mapping!"
	$(CXX) $(CXXFLAGS) -c mapping.cpp
	$(CXX) -o mapping mapping.o
	@echo "Mapping!"
	@./mapping $(FROM) $(TO)

clean:
	$(RM) $(OBJ) $(TARGET) mapping.o mapping

