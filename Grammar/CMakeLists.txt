cmake_minimum_required(VERSION 3.14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_VERBOSE_MAKEFILE ON)

set(
		FILES_GENERATED_PROPERTIES
		PARENT_SCOPE
)


set(
		LEXER_OUTPUT
		"${CMAKE_CURRENT_BINARY_DIR}/Lexer.ll.cpp"
)

set(
		LEXER_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/Lexer.l
)

### lex target ####
add_custom_command(OUTPUT ${LEXER_OUTPUT}
		COMMAND lex -o ${LEXER_OUTPUT}  ${LEXER_FILE}
		# make it compatible with c++17 :)
		COMMAND sed -i "" -- "s/register//g" ${LEXER_OUTPUT}
		DEPENDS ${LEXER_FILE}
		COMMENT "generating lexer"
		VERBATIM
		)

add_custom_target(lexer DEPENDS ${LEXER_OUTPUT} VERBATIM)

####################
set(
		PARSER_SOURCE_FILE
		"${CMAKE_CURRENT_BINARY_DIR}/Parser.yy.cpp"
)
set(
		PARSER_FILE
		${CMAKE_CURRENT_SOURCE_DIR}/Grammar.y
)
### yacc target ###
add_custom_command(OUTPUT ${PARSER_SOURCE_FILE}
		COMMAND yacc -d -o ${PARSER_SOURCE_FILE} ${PARSER_FILE}
		DEPENDS "${PARSER_FILE}"
		COMMENT "generating parser"
		VERBATIM
		)


add_custom_target(parser DEPENDS ${PARSER_SOURCE_FILE} VERBATIM)
####################

set(
		FILES_GENERATED_PROPERTIES
		${PARSER_SOURCE_FILE}
		${LEXER_OUTPUT}
		${CMAKE_CURRENT_BINARY_DIR}/Parser.yy.hpp
		PARENT_SCOPE
)


set(GRAMMAR_SOURCES
		${CMAKE_CURRENT_BINARY_DIR}/Lexer.ll.cpp
		${CMAKE_CURRENT_BINARY_DIR}/Parser.yy.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Lexer.h
		${CMAKE_CURRENT_BINARY_DIR}/Parser.yy.hpp
		PARENT_SCOPE)



add_custom_target(Grammar DEPENDS VERBATIM)

add_dependencies(parser lexer)
add_dependencies(Grammar parser)
