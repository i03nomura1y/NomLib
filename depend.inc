aes.o: aes.cpp aes.h util.h
lexer.o: lexer.cpp lexer.h regex.h util.h
regex.o: regex.cpp regex.h util.h
stdlib_c.o: stdlib_c.c stdlib_c.h
tree_base.o: tree_base.cpp
util.o: util.cpp util.h
variable.o: variable.cpp variable.h ptr_base.h util.h regex.h
varstr_c.o: varstr_c.c varstr_c.h stdlib_c.h
xml.o: xml.cpp xml.h xml_io.h xml_c.h variable.h ptr_base.h util.h \
 tree_base.h
xml_c.o: xml_c.c xml_c.h stdlib_c.h
xml_io.o: xml_io.cpp xml_io.h xml_c.h variable.h ptr_base.h
